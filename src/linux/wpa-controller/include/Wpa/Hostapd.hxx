
#ifndef HOSTAPD_HXX
#define HOSTAPD_HXX

#include <string>
#include <string_view>

#include <Wpa/IHostapd.hxx>
#include <Wpa/ProtocolHostapd.hxx>
#include <Wpa/ProtocolWpa.hxx>
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
     * @brief Reloads the interface. This will cause any settings that have been changed to take effect.
     *
     * @return true If the configuration was reloaded successfully.
     * @return false If the configuration was not reloaded successfully.
     */
    bool
    Reload() override;

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
     * @param enforceConfigurationChange When the enforce the configuration change. A value of 'Now' will trigger a configuration reload.
     * @return true The property was set successfully.
     * @return false The property was not set successfully.
     */
    bool
    SetProperty(std::string_view propertyName, std::string_view propertyValue, EnforceConfigurationChange enforceConfigurationChange = EnforceConfigurationChange::Now) override;

    /**
     * @brief Set the ssid for the interface.
     *
     * @param ssid The ssid to set.
     * @param enforceConfigurationChange When the enforce the configuration change. A value of 'Now' will trigger a configuration reload.
     * @return true If the ssid was set successfully.
     * @return false If the ssid was not set successfully.
     */
    bool
    SetSsid(std::string_view ssid, EnforceConfigurationChange enforceConfigurationChange = EnforceConfigurationChange::Now) override;

    /**
     * @brief Set the WPA protocol(s) for the interface.
     *
     * @param protocols The protocols to set.
     * @param enforceConfigurationChange When the enforce the configuration change. A value of 'Now' will trigger a configuration reload.
     * @return true If the protocols were set successfully.
     * @return false If the protocols were not set successfully.
     */
    bool
    SetWpaProtocols(std::vector<WpaProtocol> protocols, EnforceConfigurationChange enforceConfigurationChange = EnforceConfigurationChange::Now) override;

    /**
     * @brief Set the Key Management object
     *
     * @param keyManagements The key management value(s) to set.
     * @param enforceConfigurationChange When the enforce the configuration change. A value of 'Now' will trigger a configuration reload.
     * @return true The key management value(s) were set successfully.
     * @return false The key management value(s) were not set successfully.
     */
    bool
    SetKeyManagement(std::vector<WpaKeyManagement> keyManagements, EnforceConfigurationChange enforceConfigurationChange = EnforceConfigurationChange::Now) override;

private:
    const std::string m_interface;
    WpaController m_controller;
};
} // namespace Wpa

#endif // HOSTAPD_HXX
