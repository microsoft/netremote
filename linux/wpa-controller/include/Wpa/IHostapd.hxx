
#ifndef I_HOSTAPD_HXX
#define I_HOSTAPD_HXX

#include <exception>
#include <string_view>

#include <Wpa/ProtocolHostapd.hxx>

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
     * @brief Get the status for the interface.
     * 
     * @return HostapdStatus 
     */
    virtual HostapdStatus GetStatus() = 0;

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

/**
 * @brief Generic exception that may be thrown by any of the functions in
 * IHostapd.
 */
struct HostapdException : std::exception
{
    HostapdException(std::string_view message) :
        m_message(message)
    {
    }

    const char* what() const noexcept override
    {
        return m_message.c_str();
    }

private:
    const std::string m_message;
};
} // namespace Wpa

#endif // I_HOSTAPD_HXX
