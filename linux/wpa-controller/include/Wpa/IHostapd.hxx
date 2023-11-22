
#ifndef I_HOSTAPD_HXX
#define I_HOSTAPD_HXX

#include <string_view>

namespace Wpa
{
/**
 * @brief Interface for interacting with the hostapd daemon.
 * 
 * The documentation for the control interface is sparse, so the source code
 * hostapd_ctrl_iface_receive_process() was used as a reference:
 * https://w1.fi/cgit/hostap/tree/hostapd/ctrl_iface.c?h=hostap_2_10#n3503.
 */
struct IHostapd
{
    /**
     * @brief Default destructor supporting polymorphic destruction.
     */
    virtual ~IHostapd() = default;

    /**
     * @brief Get the name of the interface hostapd is managing.
     * 
     * @return std::string_view 
     */
    virtual std::string_view GetInterface() = 0;

    /**
     * @brief Checks connectivity to the hostapd daemon.
     */
    virtual bool Ping() = 0;

    /**
     * 
     * @return true 
     * @return false 
     */

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
    virtual bool IsEnabled(bool forceCheck) = 0;

    /**
     * @brief Enables the interface for use.
     * 
     * @return true 
     * @return false 
     */
    virtual bool Enable() = 0;

    /**
     * @brief Disables the interface for use.
     * 
     * @return true 
     * @return false 
     */
    virtual bool Disable() = 0;

    /**
     * @brief Terminates the process hosting the daemon.
     * 
     * @return true 
     * @return false 
     */
    virtual bool Terminate() = 0;
};
} // namespace Wpa

#endif // I_HOSTAPD_HXX
