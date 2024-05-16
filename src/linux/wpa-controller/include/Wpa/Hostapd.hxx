
#ifndef HOSTAPD_HXX
#define HOSTAPD_HXX

#include <string>
#include <string_view>

#include <Wpa/IHostapd.hxx>
#include <Wpa/ProtocolHostapd.hxx>
#include <Wpa/ProtocolWpa.hxx>
#include <Wpa/WpaController.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>

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
     * @brief Determines if the specified interface is being managed by hostapd.
     *
     * @param interfaceName The interface to check.
     * @return true If hostapd is actively managing the interface.
     * @return false If hostapd is not managing the interface.
     */
    static bool
    IsManagingInterface(std::string_view interfaceName) noexcept;

    /**
     * @brief Determines if hostapd is active for this interface.
     *
     * @return true If a hostapd daemon is active for this interface.
     * @return false If a hostapd daemon is not active for this interface.
     */
    bool
    IsActive() const noexcept;

    /**
     * @brief Enables the interface for use.
     */
    void
    Enable() override;

    /**
     * @brief Disables the interface for use.
     */
    void
    Disable() override;

    /**
     * @brief Terminates the process hosting the daemon.
     */
    void
    Terminate() override;

    /**
     * @brief Checks connectivity to the hostapd daemon.
     */
    void
    Ping() override;

    /**
     * @brief Reloads the interface. This will cause any settings that have been changed to take effect.
     */
    void
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
     * @return std::string The property string value.
     */
    std::string
    GetProperty(std::string_view propertyName) override;

    /**
     * @brief Set a property on the interface.
     *
     * @param propertyName The name of the property to set.
     * @param propertyValue The value of the property to set.
     * @param enforceConfigurationChange When to enforce the configuration change. A value of 'Now' will trigger a configuration reload.
     */
    void
    SetProperty(std::string_view propertyName, std::string_view propertyValue, EnforceConfigurationChange enforceConfigurationChange = EnforceConfigurationChange::Now) override;

    /**
     * @brief Set the ssid for the interface.
     *
     * @param ssid The ssid to set.
     * @param enforceConfigurationChange When to enforce the configuration change. A value of 'Now' will trigger a configuration reload.
     */
    void
    SetSsid(std::string_view ssid, EnforceConfigurationChange enforceConfigurationChange = EnforceConfigurationChange::Now) override;

    /**
     * @brief Set the authentication algorithm(s) for the interface.
     *
     * @param algorithms The set of allowed authentication algorithms.
     * @param enforceConfigurationChange When to enforce the configuration change. A value of 'Now' will trigger a configuration reload.
     */
    void
    SetAuthenticationAlgorithms(std::vector<WpaAuthenticationAlgorithm> algorithms, EnforceConfigurationChange enforceConfigurationChange) override;

    /**
     * @brief Set the WPA protocol(s) for the interface.
     *
     * @param protocols The protocols to set.
     * @param enforceConfigurationChange When to enforce the configuration change. A value of 'Now' will trigger a configuration reload.
     */
    void
    SetWpaSecurityProtocols(std::vector<WpaSecurityProtocol> protocols, EnforceConfigurationChange enforceConfigurationChange = EnforceConfigurationChange::Now) override;

    /**
     * @brief Set the allowed key management algorithms for the interfacallowed key management algorithms for the interface.
     *
     * @param keyManagements The key management value(s) to set.
     * @param enforceConfigurationChange When to enforce the configuration change. A value of 'Now' will trigger a configuration reload.
     */
    void
    SetKeyManagement(std::vector<WpaKeyManagement> keyManagements, EnforceConfigurationChange enforceConfigurationChange = EnforceConfigurationChange::Now) override;

    /**
     * @brief Set the allowed pairwise cipher suites for the interface.
     *
     * @param protocol The WPA protocol to set the cipher suites for.
     * @param pairwiseCiphers The ciphers to allow.
     * @param enforceConfigurationChange When to enforce the configuration change. A value of 'Now' will trigger a configuration reload.
     */
    void
    SetPairwiseCipherSuites(WpaSecurityProtocol protocol, std::vector<WpaCipher> pairwiseCiphers, EnforceConfigurationChange enforceConfigurationChange) override;

    /**
     * @brief Set the allowed pairwise cipher suites for the interface.
     *
     * @param protocolPairwiseCipherMap Map specifying the pairwise ciphers to allow for each protocol.
     * @param enforceConfigurationChange When to enforce the configuration change. A value of 'Now' will trigger a configuration reload.
     */
    void
    SetPairwiseCipherSuites(std::unordered_map<WpaSecurityProtocol, std::vector<WpaCipher>> protocolPairwiseCipherMap, EnforceConfigurationChange enforceConfigurationChange) override;

    /**
     * @brief Set the pre-shared key for the interface.
     *
     * One of either a 8-63 character passphrase, or a 64-character hex string may be specified.
     *
     * @param preSharedKey The pre-shared key to set.
     * @param enforceConfigurationChange When to enforce the configuration change. A value of 'Now' will trigger a configuration reload.
     */
    void
    SetPreSharedKey(WpaPreSharedKey preSharedKey, EnforceConfigurationChange enforceConfigurationChange) override;

    /**
     * @brief Set the accepted SAE passwords for the interface.
     *
     * @param saePasswords The SAE passwords to set.
     * @param enforceConfigurationChange When to enforce the configuration change. A value of 'Now' will trigger a configuration reload.
     */
    void
    SetSaePasswords(std::vector<SaePassword> saePasswords, EnforceConfigurationChange enforceConfigurationChange = EnforceConfigurationChange::Now) override;

    /**
     * @brief Add an SAE password.
     *
     * @param saePassword The SAE password to add.
     * @param enforceConfigurationChange When to enforce the configuration change. A value of 'Now' will trigger a configuration reload.
     */
    void
    AddSaePassword(SaePassword saePassword, EnforceConfigurationChange enforceConfigurationChange = EnforceConfigurationChange::Defer) override;

    /**
     * @brief Generates a new network access server identifier. If no length is specified, a default value will be used.
     *
     * @param lengthRequested The requested length of the identifier. Valid values are in the range [1, 48].
     * @return std::string
     */
    static std::string
    GenerateNetworkAccessServerId(std::size_t lengthRequested = Microsoft::Net::Wifi::Ieee80211FtR0KeyHolderIdLengthOctetsMaximum);

    /**
     * @brief Set the Network Access Server Id ("nas_identifier") for the access point.
     *
     * @param networkAccessServiceId The network access server identifier to set.
     */
    void
    SetNetworkAccessServerId(std::string_view networkAccessServiceId = GenerateNetworkAccessServerId());

private:
    const std::string m_interface;
    WpaController m_controller;
};
} // namespace Wpa

#endif // HOSTAPD_HXX
