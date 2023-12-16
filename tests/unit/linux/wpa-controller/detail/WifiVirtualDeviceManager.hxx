
#ifndef WIFI_VIRTUAL_DEVICE_MANAGER_HXX
#define WIFI_VIRTUAL_DEVICE_MANAGER_HXX

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

/**
 * @brief Manages virtual wlan devices.
 *
 * Note: this class is not thread-safe.
 */
struct WifiVirtualDeviceManager
{
    /**
     * @brief Driver name for the mac80211_hwsim driver.
     */
    static constexpr auto DriverMac80211HwsimName = "mac80211_hwsim";

    /**
     * @brief Default driver used if none is specified.
     */
    static constexpr auto DriverDefault = DriverMac80211HwsimName;

    /**
     * @brief Create a certain number of virtual wlan interfaces using the specified driver.
     *
     * @param numberOfInterfaces The number of virtual wlan interfaces to create.
     * @param driverName The driver to use to create the virtual wlan interfaces.
     * @return std::unordered_set<std::string>
     */
    std::unordered_set<std::string>
    CreateInterfaces(uint32_t numberOfInterfaces, std::string_view driverName = DriverDefault);

    /**
     * @brief Get a list of all virtual wlan interfaces.
     *
     * @return std::unordered_map<std::string_view, std::unordered_set<std::string>>
     */
    std::unordered_map<std::string_view, std::unordered_set<std::string>>
    EnumerateInterfaces() const;

    /**
     * @brief Remove all virtual wlan interfaces for the specified driver.
     *
     * @param driverName The driver name to remove the virtual wlan interfaces for.
     * @return true
     * @return false
     */
    bool
    RemoveInterfaces(std::string_view driverName = DriverDefault);

    /**
     * @brief Remove all virtual wlan interfaces for all drivers.
     */
    bool
    RemoveAllInterfaces();

public:
    /**
     * @brief Create a certain number of virtual wlan interfaces using the specified driver.
     *
     * @param numberOfInterfaces The number of virtual wlan interfaces to create.
     * @param driverName The driver to use to create the virtual wlan interfaces.
     * @return std::unordered_set<std::string> A list of interface names that were created.
     */
    static std::unordered_set<std::string>
    CreateInterfacesForDriver(uint32_t numberOfInterfaces, std::string_view driverName);

    /**
     * @brief Remove all virtual wlan interfaces for the specified driver.
     *
     * @param driverName The driver name to remove the virtual wlan interfaces for.
     * @return true All virtual wlan interfaces for the specified driver were removed.
     * @return false Not all virtual wlan interfaces for the specified driver were removed.
     */
    static bool
    RemoveInterfacesForDriver(std::string_view driverName);

private:
    std::unordered_map<std::string_view, std::unordered_set<std::string>> m_interfaces;
};

#endif // WIFI_VIRTUAL_DEVICE_MANAGER_HXX
