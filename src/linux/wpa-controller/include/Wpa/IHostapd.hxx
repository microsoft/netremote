
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
     * @brief Enables the interface for use.
     *
     * @return true
     * @return false
     */
    virtual bool
    Enable() = 0;

    /**
     * @brief Disables the interface for use.
     *
     * @return true
     * @return false
     */
    virtual bool
    Disable() = 0;

    /**
     * @brief Terminates the process hosting the daemon.
     *
     * @return true
     * @return false
     */
    virtual bool
    Terminate() = 0;

    /**
     * @brief Checks connectivity to the hostapd daemon.
     */
    virtual bool
    Ping() = 0;

    /**
     * @brief Get the name of the interface hostapd is managing.
     *
     * @return std::string_view
     */
    virtual std::string_view
    GetInterface() = 0;

    /**
     * @brief Get the status for the interface.
     *
     * @return HostapdStatus
     */
    virtual HostapdStatus
    GetStatus() = 0;

    /**
     * @brief Get a property value for the interface.
     *
     * @param propertyName The name of the property to retrieve.
     * @param propertyValue The string to store the property value in.
     * @return true If the property value was obtained and its value is in 'propertyValue'.
     * @return false If t he property value could not be obtained due to an error.
     */
    virtual bool
    GetProperty(std::string_view propertyName, std::string& propertyValue) = 0;

    /**
     * @brief Set a property on the interface.
     *
     * @param propertyName The name of the property to set.
     * @param propertyValue The value of the property to set.
     * @return true The property was set successfully.
     * @return false The property was not set successfully.
     */
    virtual bool
    SetProperty(std::string_view propertyName, std::string_view propertyValue) = 0;
};

/**
 * @brief Generic exception that may be thrown by any of the functions in
 * IHostapd.
 */
struct HostapdException : 
    public std::exception
{
    HostapdException(std::string_view message);

    const char*
    what() const noexcept override;

private:
    const std::string m_message;
};
} // namespace Wpa

#endif // I_HOSTAPD_HXX
