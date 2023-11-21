
#include <cstdlib>
#include <format>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include <magic_enum.hpp>
#include <signal.h>
#include "WpaDaemonManager.hxx"

namespace detail
{
/**
 * @brief Default hostapd configuration file contents.
 * 
 * This configuration file supports WPA2 with typical security settings. It is
 * meant to be used with a virtualized wlan device created with the
 * mac80211_hwsim kernel module. The intention is to use this for basic control
 * socket tests, not to exercise specifically wlan functionality.
 * 
 * This string must be formatted with std::format() to provide the interface
 * name as the first interpolation argument.
 */
static constexpr auto WpaDaemonHostapdConfigurationFileContentsFormat = R"CONFIG(
interface={}
driver=nl80211
ctrl_interface=/var/run/hostapd
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
 * @param configurationFile The file stream to write the configuration file to.
 */
void WriteDefaultConfigurationFileContents(Wpa::WpaType wpaType, std::string_view interfaceName, std::ofstream& configurationFile)
{
    switch (wpaType)
    {
    case Wpa::WpaType::Hostapd:
    {
        configurationFile << std::format(WpaDaemonHostapdConfigurationFileContentsFormat, interfaceName);
        break;
    }
    default:
    {
        throw std::runtime_error(std::format("Unsupported wpa daemon type '{}'", magic_enum::enum_name(wpaType)));
    }
    }
}
} // namespace detail

/* static */
std::filesystem::path WpaDaemonManager::CreateAndWriteDefaultConfigurationFile(Wpa::WpaType wpaType, std::string_view interfaceName)
{
    // Determine which daemon to create the configuration file for.
    const auto daemon = Wpa::GetWpaTypeDaemonBinaryName(wpaType);
    const auto daemonConfigurationFilePath = std::filesystem::temp_directory_path() / std::format("{}.conf", daemon);

    // Create the configuration file.
    const auto daemonConfigurationFileStatus = std::filesystem::status(daemonConfigurationFilePath);
    if (std::filesystem::exists(daemonConfigurationFileStatus))
    {
        return daemonConfigurationFilePath;
    }

    // Write default configuration file contents.
    std::ofstream daemonConfigurationFile{ daemonConfigurationFilePath };
    detail::WriteDefaultConfigurationFileContents(wpaType, interfaceName, daemonConfigurationFile);
    daemonConfigurationFile.flush();
    daemonConfigurationFile.close();

    std::cout << std::format("Created default configuration file for wpa '{}' daemon at {}\n", daemon, daemonConfigurationFilePath.c_str());
    return daemonConfigurationFilePath;
}

/* static */
std::optional<WpaDaemonInstanceHandle> WpaDaemonManager::Start(Wpa::WpaType wpaType, std::string_view interfaceName, const std::filesystem::path& configurationFilePath, std::string_view extraCommandLineArguments)
{
    // Use the default interface name if none provided.
    if (std::empty(interfaceName))
    {
        interfaceName = WpaDaemonManager::InterfaceNameDefault;
    }

    // Determine which daemon to start and formulate daemon binary arguments.
    const auto daemon = Wpa::GetWpaTypeDaemonBinaryName(wpaType);
    const auto configurationFileArgumentPrefix = (wpaType == Wpa::WpaType::WpaSupplicant) ? "-c" : "";
    const auto pidFilePath{ std::filesystem::temp_directory_path() / std::format("{}.pid", daemon) };

    // Start the daemon. Arguments are:
    // -B -> run in background
    // -P -> write pid to file
    // -i -> interface name
    const auto daemonStartCommand = std::format("{} -B -P {} -i {} {} {} {}", daemon, pidFilePath.c_str(), interfaceName, extraCommandLineArguments, configurationFileArgumentPrefix, configurationFilePath.c_str());
    std::cout << std::format("Starting wpa daemon with command '{}'\n", daemonStartCommand);

    int ret = std::system(daemonStartCommand.c_str());
    if (ret == -1)
    {
        ret = WEXITSTATUS(ret);
        std::cerr << std::format("Failed to start wpa '{}' daemon, ret={}\n", daemon, ret);
        return std::nullopt;
    }

    // Open the pid file on the running daemon.
    std::ifstream pidFile{ pidFilePath };
    if (!pidFile.is_open())
    {
        std::cerr << std::format("Failed to open pid file for wpa '{}' daemon\n", daemon);
        return std::nullopt;
    }

    // Read the pid from the daemon pid file.
    pid_t pid{0};
    std::stringstream pidFileContents{};
    pidFileContents << pidFile.rdbuf();
    pidFileContents >> pid;
    if (pid == 0 || pidFileContents.fail())
    {
        std::cerr << std::format("Failed to read pid file {} for wpa '{}' daemon\n", pidFilePath.c_str(), daemon);
        return std::nullopt;
    }

    std::cout << std::format("Started wpa '{}' daemon with pid {}\n", daemon, pid);

    // Return a handle to the daemon instance.
    return WpaDaemonInstanceHandle{ 
        .WpaType = wpaType,
        .Pid = pid
    };
}

/* static */
std::optional<WpaDaemonInstanceHandle> WpaDaemonManager::StartDefault(Wpa::WpaType wpaType, std::string_view interfaceName)
{
    auto configurationFilePath = CreateAndWriteDefaultConfigurationFile(wpaType, interfaceName);
    if (configurationFilePath.empty())
    {
        std::cerr << std::format("Failed to create default configuration file for wpa '{}' daemon\n", Wpa::GetWpaTypeDaemonBinaryName(wpaType));
        return std::nullopt;
    }

    return Start(wpaType, interfaceName, configurationFilePath);
}

/* static */
bool WpaDaemonManager::Stop(const WpaDaemonInstanceHandle& instanceHandle)
{
    // Kill the process associated with the daemon instance.
    int ret = kill(instanceHandle.Pid, SIGTERM);
    if (ret != 0)
    {
        std::cerr << std::format("Failed to stop wpa '{}' daemon, ret={}\n", magic_enum::enum_name(instanceHandle.WpaType), ret);
        return false;
    }

    const auto daemon = Wpa::GetWpaTypeDaemonBinaryName(instanceHandle.WpaType);
    std::cout << std::format("Stopped wpa '{}' daemon with pid {}\n", daemon, instanceHandle.Pid);

    return true;
}
