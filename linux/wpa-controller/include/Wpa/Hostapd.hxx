
#ifndef HOSTAPD_HXX
#define HOSTAPD_HXX

#include <string_view>
#include <string>

#include <Wpa/IHostapd.hxx>
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
    Hostapd(std::string_view interfaceName);

    /**
     * @brief Get the name of the interface hostapd is managing.
     * 
     * @return std::string_view 
     */
    std::string_view GetInterface() override;

    /**
     * @brief Checks connectivity to the hostapd daemon.
     */
    bool Ping() override;

    /**
     * @brief Determines if the interface is enabled for use.
     * 
     * @return true 
     * @return false 
     */
    bool IsEnabled() override;

    /**
     * @brief Enables the interface for use.
     * 
     * @return true 
     * @return false 
     */
    bool Enable() override;

    /**
     * @brief Disables the interface for use.
     * 
     * @return true 
     * @return false 
     */
    bool Disable() override;

private:
    bool m_isEnabled{false};
    const std::string m_interface;
    WpaController m_controller;
};
} // namespace Wpa

#endif // HOSTAPD_HXX
