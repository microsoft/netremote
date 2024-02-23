
#ifndef ACCESS_POINT_CONTROLLER_LINUX_HXX
#define ACCESS_POINT_CONTROLLER_LINUX_HXX

#include <memory>
#include <string_view>
#include <vector>

#include <Wpa/Hostapd.hxx>
#include <microsoft/net/wifi/AccessPointController.hxx>
#include <microsoft/net/wifi/AccessPointOperationStatus.hxx>
#include <microsoft/net/wifi/IAccessPointController.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <microsoft/net/wifi/Ieee80211AccessPointCapabilities.hxx>

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
    AccessPointControllerLinux&
    operator=(const AccessPointControllerLinux&) = delete;
    AccessPointControllerLinux(AccessPointControllerLinux&&) = delete;
    AccessPointControllerLinux&
    operator=(AccessPointControllerLinux&&) = delete;

    /**
     * @brief Get the access point operational state.
     *
     * @param operationalState The value to store the operational state.
     * @return AccessPointOperationStatus
     */
    AccessPointOperationStatus
    GetOperationalState(AccessPointOperationalState& operationalState) override;

    /**
     * @brief Get the capabilities of the access point.
     *
     * @param ieee80211AccessPointCapabilities The value to store the capabilities.
     * @return AccessPointOperationStatus
     */
    AccessPointOperationStatus
    GetCapabilities(Ieee80211AccessPointCapabilities& ieee80211AccessPointCapabilities) override;

    /**
     * @brief Set the operational state of the access point.
     *
     * @param operationalState The desired operational state.
     * @return AccessPointOperationStatus
     */
    AccessPointOperationStatus
    SetOperationalState(AccessPointOperationalState operationalState) override;

    /**
     * @brief Set the Ieee80211 protocol.
     *
     * @param ieeeProtocol The Ieee80211 protocol to be set.
     * @return AccessPointOperationStatus
     */
    AccessPointOperationStatus
    SetProtocol(Ieee80211Protocol ieeeProtocol) override;

    /**
     * @brief Set the frquency bands the access point should enable.
     *
     * @param frequencyBands The frequency bands to be set.
     * @return true
     * @return false
     */
    bool
    SetFrequencyBands(std::vector<Ieee80211FrequencyBand> frequencyBands) override;

    /**
     * @brief Set the SSID of the access point.
     *
     * @param ssid The SSID to be set.
     * @return AccessPointOperationStatus
     */
    AccessPointOperationStatus
    SetSssid(std::string_view ssid) override;

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
