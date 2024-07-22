
#ifndef ACCESS_POINT_CONTROLLER_LINUX_HXX
#define ACCESS_POINT_CONTROLLER_LINUX_HXX

#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <Wpa/Hostapd.hxx>
#include <microsoft/net/Ieee8021xRadiusAuthentication.hxx>
#include <microsoft/net/wifi/AccessPointController.hxx>
#include <microsoft/net/wifi/AccessPointOperationStatus.hxx>
#include <microsoft/net/wifi/IAccessPointController.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <microsoft/net/wifi/Ieee80211AccessPointCapabilities.hxx>
#include <microsoft/net/wifi/Ieee80211Authentication.hxx>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Implementation of IAccessPointController which uses the hostapd daemon to control the access point.
 */
struct AccessPointControllerLinux :
    public AccessPointController
{
    ~AccessPointControllerLinux() override = default;

    AccessPointControllerLinux() = delete;

    /**
     * @brief Construct a new AccessPointControllerLinux object for the specified interface.
     *
     * @param interfaceName The name of the interface to control.
     */
    explicit AccessPointControllerLinux(std::string_view interfaceName);

    /**
     * Prevent copying and moving of this object.
     */
    AccessPointControllerLinux(const AccessPointControllerLinux&) = delete;

    AccessPointControllerLinux(AccessPointControllerLinux&&) = delete;

    AccessPointControllerLinux&
    operator=(const AccessPointControllerLinux&) = delete;

    AccessPointControllerLinux&
    operator=(AccessPointControllerLinux&&) = delete;

    /**
     * @brief Get the access point operational state.
     *
     * @param operationalState The value to store the operational state.
     * @return AccessPointOperationStatus
     */
    AccessPointOperationStatus
    GetOperationalState(AccessPointOperationalState& operationalState) noexcept override;

    /**
     * @brief Get the capabilities of the access point.
     *
     * @param ieee80211AccessPointCapabilities The value to store the capabilities.
     * @return AccessPointOperationStatus
     */
    AccessPointOperationStatus
    GetCapabilities(Ieee80211AccessPointCapabilities& ieee80211AccessPointCapabilities) noexcept override;

    /**
     * @brief Set the operational state of the access point.
     *
     * @param operationalState The desired operational state.
     * @return AccessPointOperationStatus
     */
    AccessPointOperationStatus
    SetOperationalState(AccessPointOperationalState operationalState) noexcept override;

    /**
     * @brief Set the Ieee80211 PHY type.
     *
     * @param ieeePhyType The Ieee80211 PHY type to be set.
     * @return AccessPointOperationStatus
     */
    AccessPointOperationStatus
    SetPhyType(Ieee80211PhyType ieeePhyType) noexcept override;

    /**
     * @brief Set the frquency bands the access point should enable.
     *
     * @param frequencyBands The frequency bands to be set.
     * @return AccessPointOperationStatus
     */
    AccessPointOperationStatus
    SetFrequencyBands(std::vector<Ieee80211FrequencyBand> frequencyBands) noexcept override;

    /**
     * @brief Set the authentication algorithms the access point should enable.
     *
     * @param authenticationAlgorithms The authentication algorithms to be allow.
     * @return AccessPointOperationStatus
     */
    AccessPointOperationStatus
    SetAuthenticationAlgorithms(std::vector<Ieee80211AuthenticationAlgorithm> authenticationAlgorithms) noexcept override;

    /**
     * @brief Set the authentication data the access point should use.
     *
     * @param authenticationData The authentication data to be set.
     * @return AccessPointOperationStatus
     */
    AccessPointOperationStatus
    SetAuthenticationData(Ieee80211AuthenticationData authenticationData) noexcept override;

    /**
     * @brief Set the authentication and key management (akm) suites the access point should enable.
     *
     * @param akmSuites The akm suites to be allowed.
     * @return AccessPointOperationStatus
     */
    AccessPointOperationStatus
    SetAkmSuites(std::vector<Ieee80211AkmSuite> akmSuites) noexcept override;

    /**
     * @brief Set the pairwise cipher suites the access point should enable. These are used to encrypt unicast packets.
     *
     * @param pairwiseCipherSuites The pairwise cipher suites to enable.
     * @return AccessPointOperationStatus
     */
    AccessPointOperationStatus
    SetPairwiseCipherSuites(std::unordered_map<Ieee80211SecurityProtocol, std::vector<Ieee80211CipherSuite>> pairwiseCipherSuites) noexcept override;

    /**
     * @brief Set the SSID of the access point.
     *
     * @param ssid The SSID to be set.
     * @return AccessPointOperationStatus
     */
    AccessPointOperationStatus
    SetSsid(std::string_view ssid) noexcept override;

    /**
     * @brief Set the network bridge interface the access point interface will be added to.
     *
     * @param networkBridgeId The network bridge interface id. The specific format of the id is platform dependent.
     * @return AccessPointOperationStatus
     */
    AccessPointOperationStatus
    SetNetworkBridge(std::string_view networkBridgeId) noexcept override;

    /**
     * @brief Set the RADIUS configuration for the access point.
     *
     * @param radiusConfiguration The RADIUS configuration to be set.
     * @return AccessPointOperationStatus
     */
    AccessPointOperationStatus
    SetRadiusConfiguration(Microsoft::Net::Ieee8021xRadiusConfiguration radiusConfiguration) noexcept override;

private:
    Wpa::Hostapd m_hostapd;
};

/**
 * @brief Factory to create AccessPointControllerLinux objects.
 */
struct AccessPointControllerLinuxFactory :
    public IAccessPointControllerFactory
{
    AccessPointControllerLinuxFactory() = default;

    ~AccessPointControllerLinuxFactory() override = default;

    /**
     * Prevent copying and moving of this object.
     */
    AccessPointControllerLinuxFactory(const AccessPointControllerLinuxFactory&) = delete;

    AccessPointControllerLinuxFactory(AccessPointControllerLinuxFactory&&) = delete;

    AccessPointControllerLinuxFactory&
    operator=(const AccessPointControllerLinuxFactory&) = delete;

    AccessPointControllerLinuxFactory&
    operator=(AccessPointControllerLinuxFactory&&) = delete;

    /**
     * @brief Create a new IAccessPointController object.
     *
     * @return std::unique_ptr<IAccessPointController>
     */
    std::unique_ptr<IAccessPointController>
    Create(std::string_view interfaceName) override;
};
} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_CONTROLLER_LINUX_HXX
