
#include <cstdlib>
#include <format>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include <magic_enum.hpp>
#include "WpaDaemonManager.hxx"

/* static */
std::optional<WpaDaemonInstanceHandle> WpaDaemonManager::Start(Wpa::WpaType wpaType, const std::string& interfaceName, const std::string& commandLineArguments)
{
    // Note: The user running the test must have root privileges to execute the below command, either directly (root uid) or as a sudoer.
    // Load the mac80211_hwsim kernel module to simulate a wireless interface.
    int ret = std::system("sudo modprobe mac80211_hwsim radios=1");
    if (ret != 0)
    {
        std::cerr << std::format("Failed to load mac80211_hwsim kernel module, ret={}\n", ret);
        return std::nullopt;
    }

    // Determine which daemon to start.
    const auto wpaDaemon = Wpa::GetWpaTypeDaemonBinaryName(wpaType);
    const auto arguments = !std::empty(commandLineArguments)
        ? commandLineArguments
        : std::format("-c {}.conf {}", wpaDaemon, commandLineArguments);
    const auto pidFilePath{ std::filesystem::temp_directory_path() / std::format("{}.pid", wpaDaemon) };

    // Start the daemon.
    const auto wpaDaemonStartCommand = std::format("{} -B -P {} -i {} {}", wpaDaemon, pidFilePath.c_str(), interfaceName, arguments);
    ret = std::system(wpaDaemonStartCommand.c_str());
    if (ret != 0)
    {
        std::cerr << std::format("Failed to start wpa {} daemon, ret={}\n", magic_enum::enum_name(wpaType), ret);
        return std::nullopt;
    }

    WpaDaemonInstanceHandle instanceHandle{ wpaType, std::nullopt };

    // Get the pid of the running daemon.
    std::ifstream pidFile{ pidFilePath };
    if (pidFile.is_open())
    {
        pid_t pid{0};
        std::stringstream pidFileContents{};
        pidFileContents << pidFile.rdbuf();
        pidFileContents >> pid;

        if (pid != 0 && !pidFileContents.fail())
        {
            instanceHandle.Pid = pid;
        }
    }

    // Return a handle to the daemon instance.
    return std::move(instanceHandle);
}

/* static */
bool WpaDaemonManager::Stop(const WpaDaemonInstanceHandle& instanceHandle)
{
    // Kill the process, or all processes matching the name if the pid is not known.
    int ret = (instanceHandle.Pid.has_value())
        ? std::system(std::format("kill {}", instanceHandle.Pid.value()).c_str())
        : std::system(std::format("killall -I {}", Wpa::GetWpaTypeDaemonBinaryName(instanceHandle.WpaType)).c_str());
    if (ret != 0)
    {
        std::cerr << std::format("Failed to stop wpa {} daemon, ret={}\n", magic_enum::enum_name(instanceHandle.WpaType), ret);
        return false;
    }

    return true;
}
