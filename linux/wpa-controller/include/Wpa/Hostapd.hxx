
#ifndef HOSTAPD_HXX
#define HOSTAPD_HXX

#include <shared_mutex>
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
     * @param forceCheck Whether or not the interface should be probed for its
     * state. When this is false, the cached state will be used. Otherwise, it
     * will be determined directly by probing it from the remote daemon
     * instance.  
     * @return true 
     * @return false 
     */
    bool IsEnabled(bool forceCheck = false) override;

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

    /**
     * @brief Terminates the process hosting the daemon.
     * 
     * @return true 
     * @return false 
     */
    bool Terminate() override;

private:
    bool m_isEnabled{false};
    // Protects any state that can be modified by multiple threads including:
    // - m_isEnabled
    std::shared_mutex m_stateGate;

    const std::string m_interface;
    WpaController m_controller;
};
} // namespace Wpa

#endif // HOSTAPD_HXX
