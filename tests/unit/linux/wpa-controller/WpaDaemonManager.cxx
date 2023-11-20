
#include <cstdlib>
#include <format>
#include <iostream>

#include <magic_enum.hpp>
#include "WpaDaemonManager.hxx"

/* static */
std::optional<WpaDaemonInstanceToken> WpaDaemonManager::Start(Wpa::WpaType wpaType, const std::string interfaceName, const std::string& commandLineArguments)
{
    // Load the mac80211_hwsim kernel module to simulate a wireless interface.
    int ret = std::system("modprobe mac80211_hwsim radios=1");
    if (ret != 0)
    {
        std::cerr << std::format("Failed to load mac80211_hwsim kernel module, ret={}\n", ret);
        return std::nullopt;
    }

    // Determine which daemon to start.
    const auto wpaDaemon = wpaType == Wpa::WpaType::WpaSupplicant 
        ? "wpa_supplicant" 
        : "hostapd";
    const auto arguments = !std::empty(commandLineArguments)
        ? commandLineArguments
        : std::format("-c {}.conf {}", wpaDaemon, commandLineArguments);

    // Start the daemon.
    const auto daemonStartCommand = std::format("{} -B -i {} {}", wpaDaemon, interfaceName, arguments);
    ret = std::system(daemonStartCommand.c_str());
    if (ret != 0)
    {
        std::cerr << std::format("Failed to start wpa {} daemon, ret={}\n", magic_enum::enum_name(wpaType), ret);
        return std::nullopt;
    }

    // Return an instance token/handle to the daemon instance.
    return WpaDaemonInstanceToken{ wpaType };
}

/* static */
bool WpaDaemonManager::Stop([[maybe_unused]] const WpaDaemonInstanceToken& instanceToken)
{
    return false;
}
