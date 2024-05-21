
#ifndef I_HOSTAPD_HXX
#define I_HOSTAPD_HXX

#include <cstdint>
#include <exception>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <Wpa/ProtocolHostapd.hxx>
#include <Wpa/ProtocolWpa.hxx>

namespace Wpa
{
/**
 * @brief Syntax sugar for specifying when configuration change should be enforced.
 */
enum class EnforceConfigurationChange {
    Now,
    Defer,
};

/**
 * @brief Interface for interacting with the hostapd daemon.
 *
 * The documentation for the control interface is sparse, so the source code
 * hostapd_ctrl_iface_receive_process() was used as a reference:
 * https://w1.fi/cgit/hostap/tree/hostapd/ctrl_iface.c?h=hostap_2_10#n3503.
 */
struct IHostapd
{
    IHostapd() = default;

    /**
     * @brief Default destructor supporting polymorphic destruction.
     */
    virtual ~IHostapd() = default;

    /**
     * Prevent copying and moving of IHostapd objects.
     */
    IHostapd(const IHostapd&) = delete;

    IHostapd(IHostapd&&) = delete;

    IHostapd&
    operator=(const IHostapd&) = delete;

    IHostapd&
    operator=(IHostapd&&) = delete;

    /**
     * @brief Enables the interface for use.
     */
    virtual void
    Enable() = 0;

    /**
     * @brief Disables the interface for use.
     */
    virtual void
    Disable() = 0;

    /**
     * @brief Terminates the process hosting the daemon.
     */
    virtual void
    Terminate() = 0;

    /**
     * @brief Checks connectivity to the hostapd daemon.
     */
    virtual void
    Ping() = 0;

    /**
     * @brief Reloads the interface. This will cause any settings that have been changed to take effect.
     */
    virtual void
    Reload() = 0;

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
     * @return std::string The property string value.
     */
    virtual std::string
    GetProperty(std::string_view propertyName) = 0;

    /**
     * @brief Set a property on the interface.
     *
     * @param propertyName The name of the property to set.
     * @param propertyValue The value of the property to set.
     * @param enforceConfigurationChange When to enforce the configuration change. A value of 'Now' will trigger a configuration reload.
     */
    virtual void
    SetProperty(std::string_view propertyName, std::string_view propertyValue, EnforceConfigurationChange enforceConfigurationChange) = 0;

    /**
     * @brief Set the ssid for the interface.
     *
     * @param ssid The ssid to set.
     * @param enforceConfigurationChange When to enforce the configuration change. A value of 'Now' will trigger a configuration reload.
     */
    virtual void
    SetSsid(std::string_view ssid, EnforceConfigurationChange enforceConfigurationChange) = 0;

    /**
     * @brief Set the authentication algorithm(s) for the interface.
     *
     * @param algorithms The set of allowed authentication algorithms.
     * @param enforceConfigurationChange When to enforce the configuration change. A value of 'Now' will trigger a configuration reload.
     */
    virtual void
    SetAuthenticationAlgorithms(std::vector<WpaAuthenticationAlgorithm> algorithms, EnforceConfigurationChange enforceConfigurationChange) = 0;

    /**
     * @brief Set the WPA protocol(s) for the interface.
     *
     * @param protocols The protocols to set.
     * @param enforceConfigurationChange When to enforce the configuration change. A value of 'Now' will trigger a configuration reload.
     */
    virtual void
    SetWpaSecurityProtocols(std::vector<WpaSecurityProtocol> protocols, EnforceConfigurationChange enforceConfigurationChange) = 0;

    /**
     * @brief Set the allowed key management algorithms for the interfacallowed key management algorithms for the interface.
     *
     * @param keyManagements The key management value(s) to set.
     * @param enforceConfigurationChange When to enforce the configuration change. A value of 'Now' will trigger a configuration reload.
     */
    virtual void
    SetKeyManagement(std::vector<WpaKeyManagement> keyManagements, EnforceConfigurationChange enforceConfigurationChange) = 0;

    /**
     * @brief Set the allowed pairwise cipher suites for the interface.
     *
     * @param protocol The WPA protocol to set the cipher suites for.
     * @param ciphers The pairwise ciphers to allow.
     * @param enforceConfigurationChange When to enforce the configuration change. A value of 'Now' will trigger a configuration reload.
     */
    virtual void
    SetPairwiseCipherSuites(WpaSecurityProtocol protocol, std::vector<WpaCipher> ciphers, EnforceConfigurationChange enforceConfigurationChange) = 0;

    /**
     * @brief Set the allowed pairwise cipher suites for the interface.
     *
     * @param protocolCipherMap Map specifying the pairwise ciphers to allow for each protocol.
     * @param enforceConfigurationChange When to enforce the configuration change. A value of 'Now' will trigger a configuration reload.
     */
    virtual void
    SetPairwiseCipherSuites(std::unordered_map<WpaSecurityProtocol, std::vector<WpaCipher>> protocolCipherMap, EnforceConfigurationChange enforceConfigurationChange) = 0;

    /**
     * @brief Set the pre-shared key for the interface.
     *
     * One of either a 8-63 character passphrase, or a 64-character hex string may be specified.
     *
     * @param preSharedKey The pre-shared key to set.
     * @param enforceConfigurationChange When to enforce the configuration change. A value of 'Now' will trigger a configuration reload.
     */
    virtual void
    SetPreSharedKey(WpaPreSharedKey preSharedKey, EnforceConfigurationChange enforceConfigurationChange) = 0;

    /**
     * @brief Set the accepted SAE passwords for the interface.
     *
     * @param saePasswords The SAE passwords to set.
     * @param enforceConfigurationChange When to enforce the configuration change. A value of 'Now' will trigger a configuration reload.
     */
    virtual void
    SetSaePasswords(std::vector<SaePassword> saePasswords, EnforceConfigurationChange enforceConfigurationChange) = 0;

    /**
     * @brief Add an SAE password.
     *
     * @param saePassword The SAE password to add.
     * @param enforceConfigurationChange When to enforce the configuration change. A value of 'Now' will trigger a configuration reload.
     */
    virtual void
    AddSaePassword(SaePassword saePassword, EnforceConfigurationChange enforceConfigurationChange) = 0;

    /**
     * @brief Set the network bridge interface the access point interface will be added to.
     *
     * Note that this will only take effect after the configuration is reloaded.
     *
     * @param bridgeInterface The name of the network bridge interface.
     * @param enforceConfigurationChange When to enforce the configuration change. A value of 'Now' will trigger a
     * configuration reload.
     */
    virtual void
    SetBridgeInterface(std::string_view bridgeInterface, EnforceConfigurationChange enforceConfigurationChange) = 0;
};

/**
 * @brief Generic exception that may be thrown by any of the functions in
 * IHostapd.
 */
struct HostapdException :
    public std::exception
{
    explicit HostapdException(std::string_view message);

    const char*
    what() const noexcept override;

private:
    std::string m_message;
};
} // namespace Wpa

#endif // I_HOSTAPD_HXX
