
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

void WriteDefaultConfigurationFileContents(Wpa::WpaType wpaType, const std::string& interfaceName, std::ofstream& configurationFile)
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
std::filesystem::path WpaDaemonManager::CreateDefaultConfigurationFile(Wpa::WpaType wpaType, const std::string& interfaceName)
{
    // Determine which daemon to create the configuration file for.
    const auto wpaDaemon = Wpa::GetWpaTypeDaemonBinaryName(wpaType);
    const auto wpaDaemonConfigurationFilePath = std::filesystem::temp_directory_path() / std::format("{}.conf", wpaDaemon);

    // Create the configuration file.
    const auto wpaDaemonConfigurationFileStatus = std::filesystem::status(wpaDaemonConfigurationFilePath);
    if (std::filesystem::exists(wpaDaemonConfigurationFileStatus))
    {
        return wpaDaemonConfigurationFilePath;
    }

    // Write default configuration file contents.
    std::ofstream wpaDaemonConfigurationFile{ wpaDaemonConfigurationFilePath };
    detail::WriteDefaultConfigurationFileContents(wpaType, interfaceName, wpaDaemonConfigurationFile);
    wpaDaemonConfigurationFile.flush();
    wpaDaemonConfigurationFile.close();

    return wpaDaemonConfigurationFilePath;
}

/* static */
std::optional<WpaDaemonInstanceHandle> WpaDaemonManager::Start(Wpa::WpaType wpaType, const std::string& interfaceName, const std::string& commandLineArguments)
{
    // Note: The user running the test must have root privileges to execute the below command, either directly (root uid) or as a sudoer.
    // Load the mac80211_hwsim kernel module to simulate a wireless interface.
    int ret = std::system("sudo modprobe mac80211_hwsim radios=1");
    if (ret == -1)
    {
        ret = WEXITSTATUS(ret);
        std::cerr << std::format("Failed to load mac80211_hwsim kernel module, ret={}\n", ret);
        return std::nullopt;
    }

    // Determine which daemon to start.
    const auto wpaDaemon = Wpa::GetWpaTypeDaemonBinaryName(wpaType);
    const auto arguments = !std::empty(commandLineArguments)
        ? commandLineArguments
        : std::format("{} /etc/{}.conf {}", (wpaType == Wpa::WpaType::WpaSupplicant) ? "-c" : "", wpaDaemon, commandLineArguments);
    const auto pidFilePath{ std::filesystem::temp_directory_path() / std::format("{}.pid", wpaDaemon) };

    // Start the daemon, passing the -P option which will create a file containing its pid.
    const auto wpaDaemonStartCommand = std::format("{} -B -P {} -i {} {}", wpaDaemon, pidFilePath.c_str(), interfaceName, arguments);
    ret = std::system(wpaDaemonStartCommand.c_str());
    if (ret == -1)
    {
        ret = WEXITSTATUS(ret);
        std::cerr << std::format("Failed to start wpa '{}' daemon, ret={}\n", wpaDaemon, ret);
        return std::nullopt;
    }

    // Open the pid file on the running daemon.
    std::ifstream pidFile{ pidFilePath };
    if (!pidFile.is_open())
    {
        std::cerr << std::format("Failed to open pid file for wpa '{}' daemon\n", wpaDaemon);
        return std::nullopt;
    }

    // Read the pid from the daemon pid file.
    pid_t pid{0};
    std::stringstream pidFileContents{};
    pidFileContents << pidFile.rdbuf();
    pidFileContents >> pid;
    if (pid == 0 || pidFileContents.fail())
    {
        std::cerr << std::format("Failed to read pid file {} for wpa '{}' daemon\n", pidFilePath.c_str(), wpaDaemon);
        return std::nullopt;
    }

    std::cout << std::format("Started wpa '{}' daemon with pid {}\n", wpaDaemon, pid);

    // Return a handle to the daemon instance.
    return WpaDaemonInstanceHandle{ 
        .WpaType = wpaType,
        .Pid = pid
    };
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

    const auto wpaDaemon = Wpa::GetWpaTypeDaemonBinaryName(instanceHandle.WpaType);
    std::cout << std::format("Stopped wpa '{}' daemon with pid {}\n", wpaDaemon, instanceHandle.Pid);

    return true;
}
