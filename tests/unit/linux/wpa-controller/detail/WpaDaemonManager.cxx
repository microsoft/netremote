
#include <csignal>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <iterator>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string_view>

#include <Wpa/ProtocolWpaConfig.hxx>
#include <Wpa/WpaCore.hxx>
#include <magic_enum.hpp>
#include <plog/Log.h>

#include "HostapdBinaryInfo.hxx"
#include "WpaDaemonManager.hxx"

namespace detail
{
/**
 * @brief Format string for the default wpa_supplicant configuration file contents.
 *
 * There are 1 arguments expected to be substituted into the format string:
 *
 *  1. The control interface path.
 */
static constexpr auto WpaDaemonWpaSupplicantConfigurationFileContentsFormat = R"CONFIG(
ctrl_interface={}
)CONFIG";

/**
 * @brief Format string for the default hostapd configuration file contents.
 *
 * There are 2 argumentd expected to be substituted into the format string:
 *
 * 1. The wlan interface name.
 * 2. The control interface path.
 */
static constexpr auto WpaDaemonHostapdConfigurationFileContentsFormat = R"CONFIG(
interface={}
driver=nl80211
ctrl_interface={}
ssid=wificontrollertest
hw_mode=g
channel=1
auth_algs=3
wpa=2
wpa_passphrase=password
wpa_key_mgmt=WPA-PSK WPA-PSK-SHA256 SAE
wpa_pairwise=TKIP CCMP
rsn_pairwise=CCMP
)CONFIG";

/**
 * @brief Write the default configuration file contents for the specified wpa
 * daemon type to the specified file stream.
 *
 * @param wpaType The type of wpa daemon to write the configuration file for.
 * @param interfaceName The wlan interface the daemon will be managing.
 * @param controlSocketPath The path to the control socket for the daemon.
 * @param configurationFile The file stream to write the configuration file to.
 */
void
WriteDefaultConfigurationFileContents(Wpa::WpaType wpaType, std::string_view interfaceName, std::string_view controlSocketPath, std::ofstream& configurationFile)
{
    switch (wpaType) {
    case Wpa::WpaType::Hostapd:
        configurationFile << std::format(WpaDaemonHostapdConfigurationFileContentsFormat, interfaceName, controlSocketPath);
        break;
    case Wpa::WpaType::WpaSupplicant:
        configurationFile << std::format(WpaDaemonWpaSupplicantConfigurationFileContentsFormat, controlSocketPath);
        break;
    case Wpa::WpaType::Unknown:
        [[fallthrough]];
    default:
        throw std::runtime_error(std::format("Unsupported wpa daemon type '{}'", magic_enum::enum_name(wpaType)));
    }
}
} // namespace detail

/* static */
std::filesystem::path
WpaDaemonManager::CreateAndWriteDefaultConfigurationFile(Wpa::WpaType wpaType, std::string_view interfaceName)
{
    // Determine which daemon to create the configuration file for.
    const auto daemon = Wpa::GetWpaTypeDaemonBinaryName(wpaType);
    const auto daemonConfigurationFilePath = std::filesystem::temp_directory_path() / std::format("{}.conf", daemon);
    const auto daemonControlSocketPath{ Wpa::ProtocolWpaConfig::GetControlSocketPath(wpaType) };

    // Create and write default configuration file contents.
    std::ofstream daemonConfigurationFile{ daemonConfigurationFilePath, std::ios::out | std::ios::trunc };
    detail::WriteDefaultConfigurationFileContents(wpaType, interfaceName, daemonControlSocketPath, daemonConfigurationFile);
    daemonConfigurationFile.flush();
    daemonConfigurationFile.close();

    LOGI << std::format("Created default configuration file for wpa '{}' daemon at {}\n", daemon, daemonConfigurationFilePath.c_str());
    return daemonConfigurationFilePath;
}

/* static */
std::optional<WpaDaemonInstanceHandle>
WpaDaemonManager::Start(Wpa::WpaType wpaType, std::string_view interfaceName, const std::filesystem::path& daemonFilePath, const std::filesystem::path& configurationFilePath, std::string_view extraCommandLineArguments)
{
    // Use the default interface name if none provided.
    if (std::empty(interfaceName)) {
        interfaceName = WpaDaemonManager::InterfaceNameDefault;
    }

    // Create the control socket path if it doesn't exist.
    const std::filesystem::path controlSocketPath{ Wpa::ProtocolWpaConfig::GetControlSocketPath(wpaType) };
    if (!std::filesystem::exists(controlSocketPath)) {
        if (!std::filesystem::create_directories(controlSocketPath)) {
            LOGE << std::format("Failed to create control socket path '{}'\n", controlSocketPath.c_str());
            return std::nullopt;
        }
    }

    // Determine which daemon to start and formulate daemon binary arguments.
    const auto daemon = Wpa::GetWpaTypeDaemonBinaryName(wpaType);
    const auto* configurationFileArgumentPrefix = (wpaType == Wpa::WpaType::WpaSupplicant) ? "-c" : "";
    const auto pidFilePath{ std::filesystem::temp_directory_path() / std::format("{}.pid", daemon) };

    // Start the daemon. Arguments are:
    // -B -> run in background
    // -P -> write pid to file
    // -i -> interface name
    const auto daemonStartCommand = std::format("{} -B -P {} -i {} {} {} {}", daemonFilePath.c_str(), pidFilePath.c_str(), interfaceName, extraCommandLineArguments, configurationFileArgumentPrefix, configurationFilePath.c_str());
    LOGI << std::format("Starting wpa daemon with command '{}'\n", daemonStartCommand);

    int ret = std::system(daemonStartCommand.c_str()); // NOLINT(cert-env33-c)
    if (ret == -1) {
        ret = WEXITSTATUS(ret);
        LOGE << std::format("Failed to start wpa '{}' daemon, ret={}\n", daemon, ret);
        return std::nullopt;
    }

    // Open the pid file on the running daemon.
    std::ifstream pidFile{ pidFilePath };
    if (!pidFile.is_open()) {
        LOGE << std::format("Failed to open pid file for wpa '{}' daemon\n", daemon);
        return std::nullopt;
    }

    // Read the pid from the daemon pid file.
    pid_t pid{ 0 };
    std::stringstream pidFileContents{};
    pidFileContents << pidFile.rdbuf();
    pidFileContents >> pid;
    if (pid == 0 || pidFileContents.fail()) {
        LOGE << std::format("Failed to read pid file {} for wpa '{}' daemon\n", pidFilePath.c_str(), daemon);
        return std::nullopt;
    }

    LOGI << std::format("Started wpa '{}' daemon with pid {}\n", daemon, pid);

    // Return a handle to the daemon instance.
    return WpaDaemonInstanceHandle{
        .WpaType = wpaType,
        .Pid = pid
    };
}

/* static */
std::optional<WpaDaemonInstanceHandle>
WpaDaemonManager::StartDefault(Wpa::WpaType wpaType, std::string_view interfaceName)
{
    auto configurationFilePath = CreateAndWriteDefaultConfigurationFile(wpaType, interfaceName);
    if (configurationFilePath.empty()) {
        LOGE << std::format("Failed to create default configuration file for wpa '{}' daemon\n", Wpa::GetWpaTypeDaemonBinaryName(wpaType));
        return std::nullopt;
    }

    return Start(wpaType, interfaceName, detail::HostapdBinaryPath, configurationFilePath);
}

/* static */
bool
WpaDaemonManager::Stop(const WpaDaemonInstanceHandle& instanceHandle)
{
    // Kill the process associated with the daemon instance.
    int ret = kill(instanceHandle.Pid, SIGTERM);
    if (ret != 0) {
        LOGE << std::format("Failed to stop wpa '{}' daemon, ret={}\n", magic_enum::enum_name(instanceHandle.WpaType), ret);
        return false;
    }

    const auto daemon = Wpa::GetWpaTypeDaemonBinaryName(instanceHandle.WpaType);
    LOGI << std::format("Stopped wpa '{}' daemon with pid {}\n", daemon, instanceHandle.Pid);

    return true;
}
