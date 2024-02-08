
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "WifiVirtualDeviceManager.hxx"

std::unordered_set<std::string>
WifiVirtualDeviceManager::CreateInterfaces(uint32_t numberOfInterfaces, std::string_view driverName)
{
    auto interfacesCreated = CreateInterfacesForDriver(numberOfInterfaces, driverName);
    auto& interfacesExisting = m_interfaces[driverName];
    interfacesExisting.insert(std::cbegin(interfacesCreated), std::cend(interfacesCreated));

    return interfacesCreated;
}

std::unordered_map<std::string_view, std::unordered_set<std::string>>
WifiVirtualDeviceManager::EnumerateInterfaces() const
{
    return m_interfaces;
}

bool
WifiVirtualDeviceManager::RemoveAllInterfaces()
{
    for (auto it = std::begin(m_interfaces); it != std::end(m_interfaces);) {
        if (!RemoveInterfacesForDriver(it->first)) {
            return false;
        }

        it = m_interfaces.erase(it);
    }

    return true;
}

/* static */
bool
WifiVirtualDeviceManager::RemoveInterfaces(std::string_view driverName)
{
    return RemoveInterfacesForDriver(driverName);
}

/* static */
std::unordered_set<std::string>
WifiVirtualDeviceManager::CreateInterfacesForDriver(uint32_t numberOfInterfaces, std::string_view driverName)
{
    static constexpr auto InterfaceNamePrefixDefault = "wlan";

    // Prepare the driver modprobe arguments, if necessary.
    std::string driverArguments{};
    if (driverName == DriverMac80211HwsimName) {
        driverArguments = std::format("radios={}", numberOfInterfaces);
    }

    // Load the specified kernel module, which should create the simulated wireless interface(s).
    const auto driverLoadCommand = std::format("sudo modprobe {} {}", driverName, driverArguments);
    int ret = std::system(driverLoadCommand.c_str()); // NOLINT(cert-env33-c, cert-err34-c)
    if (ret == -1) {
        ret = WEXITSTATUS(ret);
        std::cerr << std::format("Failed to load {} driver, load command '{}' failed (ret={})\n", driverName, driverLoadCommand, ret);
        return {};
    }

    // Generate a list of interface names, assuming a default interface name
    // prefix and the interface index starts at 0.
    std::vector<std::string> interfaces(numberOfInterfaces);
    std::ranges::generate(interfaces, [i = 0]() mutable {
        return std::format("{}{}", InterfaceNamePrefixDefault, i++);
    });

    return {
        std::make_move_iterator(std::begin(interfaces)),
        std::make_move_iterator(std::end(interfaces))
    };
}

/* static */
bool
WifiVirtualDeviceManager::RemoveInterfacesForDriver(std::string_view driverName)
{
    // Forcefully remove any existing loaded kernel module for this driver.
    const auto driverUnloadCommand = std::format("sudo modprobe -rf {}", driverName);   // NOLINT(concurrency-mt-unsafe))
    int ret = std::system(driverUnloadCommand.c_str()); // NOLINT(cert-env33-c)
    if (ret == -1) {
        ret = WEXITSTATUS(ret);
        std::cerr << std::format("Failed to unload {} driver, unload command '{}' failed (ret={})\n", driverName, driverUnloadCommand, ret);
        return false;
    }

    return true;
}
