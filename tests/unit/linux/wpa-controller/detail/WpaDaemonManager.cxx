
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

#include "WpaDaemonManager.hxx"
#include <Wpa/WpaCore.hxx>
#include <magic_enum.hpp>
#include <plog/Log.h>

#include "hostapd.conf.format.hxx"

namespace detail
{

/**
 * @brief Write the default configuration file contents for the specified wpa
 * daemon type to the specified file stream.
 *
 * @param wpaType The type of wpa daemon to write the configuration file for.
 * @param interfaceName The wlan interface the daemon will be managing.
 * @param configurationFile The file stream to write the configuration file to.
 */
void
WriteDefaultConfigurationFileContents(Wpa::WpaType wpaType, std::string_view interfaceName, std::ofstream& configurationFile)
{
    switch (wpaType) {
    case Wpa::WpaType::Hostapd: {
        configurationFile << std::format(WpaDaemonHostapdConfigurationFileContentsFormat, interfaceName);
        break;
    }
    default: {
        throw std::runtime_error(std::format("Unsupported wpa daemon type '{}'", magic_enum::enum_name(wpaType)));
    }
    }
}
} // namespace detail

/* static */
std::filesystem::path
WpaDaemonManager::FindDaemonBinary(Wpa::WpaType wpaType, const std::filesystem::path& searchPath)
{
    using std::filesystem::perms;

    const auto daemon = Wpa::GetWpaTypeDaemonBinaryName(wpaType);

    LOGI << std::format("Searching for hostapd daemon binary '{}' in '{}'\n", daemon, searchPath.c_str());

    for (const auto& directoryEntry : std::filesystem::recursive_directory_iterator(searchPath)) {
        if (directoryEntry.is_regular_file() && directoryEntry.path().filename() == daemon) {
            const auto permissions = directoryEntry.status().permissions();
            if ((permissions & (perms::owner_exec | perms::group_exec | perms::others_exec)) != perms::none) {
                return directoryEntry.path();
            }
        }
    }

    return {};
}

/* static */
std::filesystem::path
WpaDaemonManager::CreateAndWriteDefaultConfigurationFile(Wpa::WpaType wpaType, std::string_view interfaceName)
{
    // Determine which daemon to create the configuration file for.
    const auto daemon = Wpa::GetWpaTypeDaemonBinaryName(wpaType);
    const auto daemonConfigurationFilePath = std::filesystem::temp_directory_path() / std::format("{}.conf", daemon);

    // Create and write default configuration file contents.
    std::ofstream daemonConfigurationFile{ daemonConfigurationFilePath, std::ios::out | std::ios::trunc };
    detail::WriteDefaultConfigurationFileContents(wpaType, interfaceName, daemonConfigurationFile);
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

    auto daemonFilePath = FindDaemonBinary(wpaType);
    if (daemonFilePath.empty()) {
        LOGE << std::format("Failed to find wpa '{}' daemon binary\n", magic_enum::enum_name(wpaType));
        return std::nullopt;
    }

    return Start(wpaType, interfaceName, daemonFilePath, configurationFilePath);
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
