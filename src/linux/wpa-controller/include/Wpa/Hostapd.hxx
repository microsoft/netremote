
#ifndef HOSTAPD_HXX
#define HOSTAPD_HXX

#include <string>
#include <string_view>

#include <Wpa/IHostapd.hxx>
#include <Wpa/ProtocolHostapd.hxx>
#include <Wpa/WpaController.hxx>

namespace Wpa
{
/**
 * @brief Concrete implementation of the IHostapd interface.
 */
struct Hostapd :
    public IHostapd
{
    /**
     * @brief Construct a new Hostapd object.
     *
     * @param interfaceName The name of the intrerface to control. Eg. wlan1.
     */
    explicit Hostapd(std::string_view interfaceName);

    /**
     * @brief Enables the interface for use.
     *
     * @return true
     * @return false
     */
    bool
    Enable() override;

    /**
     * @brief Disables the interface for use.
     *
     * @return true
     * @return false
     */
    bool
    Disable() override;

    /**
     * @brief Terminates the process hosting the daemon.
     *
     * @return true
     * @return false
     */
    bool
    Terminate() override;

    /**
     * @brief Checks connectivity to the hostapd daemon.
     */
    bool
    Ping() override;

    /**
     * @brief Get the name of the interface hostapd is managing.
     *
     * @return std::string_view
     */
    std::string_view
    GetInterface() override;

    /**
     * @brief Get the status for the interface.
     *
     * @return HostapdStatus
     */
    HostapdStatus
    GetStatus() override;

    /**
     * @brief Set the ssid of the access point.
     *
     * @param ssid The ssid to set.
     * @return true
     * @return false
     */
    bool
    SetSsid(std::string_view ssid, bool reload = true);

    /**
     * @brief Get a property value for the interface.
     *
     * @param propertyName The name of the property to retrieve.
     * @param propertyValue The string to store the property value in.
     * @return true If the property value was obtained and its value is in 'propertyValue'.
     * @return false If t he property value could not be obtained due to an error.
     */
    bool
    GetProperty(std::string_view propertyName, std::string& propertyValue) override;

    /**
     * @brief Set a property on the interface.
     *
     * @param propertyName The name of the property to set.
     * @param propertyValue The value of the property to set.
     * @return true The property was set successfully.
     * @return false The property was not set successfully.
     */
    bool
    SetProperty(std::string_view propertyName, std::string_view propertyValue) override;

    /**
     * @brief Reloads the interface.
     *
     * @return true
     * @return false
     */
    bool
    Reload() override;

private:
    const std::string m_interface;
    WpaController m_controller;
};
} // namespace Wpa

#endif // HOSTAPD_HXX
