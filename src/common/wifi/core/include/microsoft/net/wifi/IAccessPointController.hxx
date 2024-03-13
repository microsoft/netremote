
#ifndef I_ACCESS_POINT_CONTROLLER_HXX
#define I_ACCESS_POINT_CONTROLLER_HXX

#include <exception>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <microsoft/net/wifi/AccessPointOperationStatus.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <microsoft/net/wifi/Ieee80211AccessPointCapabilities.hxx>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Class allowing control of an access point.
 */
struct IAccessPointController
{
    IAccessPointController() = default;

    /**
     * @brief Destroy the IAccessPointController object.
     */
    virtual ~IAccessPointController() = default;

    /**
     * Prevent copying and moving of IAccessPointController objects.
     */
    IAccessPointController(const IAccessPointController&) = delete;

    IAccessPointController(IAccessPointController&&) = delete;

    IAccessPointController&
    operator=(const IAccessPointController&) = delete;

    IAccessPointController&
    operator=(IAccessPointController&&) = delete;

    /**
     * @brief Get the interface name associated with this controller.
     *
     * @return std::string_view
     */
    virtual std::string_view
    GetInterfaceName() const noexcept = 0;

    /**
     * @brief Get the access point operational state.
     *
     * @param operationalState The value to store the operational state.
     * @return AccessPointOperationStatus
     */
    virtual AccessPointOperationStatus
    GetOperationalState(AccessPointOperationalState& operationalState) noexcept = 0;

    /**
     * @brief Get the capabilities of the access point.
     *
     * @param ieee80211AccessPointCapabilities The value to store the capabilities.
     * @return AccessPointOperationStatus
     */
    virtual AccessPointOperationStatus
    GetCapabilities(Ieee80211AccessPointCapabilities& ieee80211AccessPointCapabilities) noexcept = 0;

    /**
     * @brief Set the operational state of the access point.
     *
     * @param operationalState The desired operational state.
     * @return AccessPointOperationStatus
     */
    virtual AccessPointOperationStatus
    SetOperationalState(AccessPointOperationalState operationalState) noexcept = 0;

    /**
     * @brief Set the Ieee80211 protocol of the access point.
     *
     * @param ieeeProtocol The Ieee80211 protocol to be set.
     * @return AccessPointOperationStatus
     */
    virtual AccessPointOperationStatus
    SetProtocol(Ieee80211Protocol ieeeProtocol) noexcept = 0;

    /**
     * @brief Set the frquency bands the access point should enable.
     *
     * @param frequencyBands The frequency bands to be set.
     * @return AccessPointOperationStatus
     */
    virtual AccessPointOperationStatus
    SetFrequencyBands(std::vector<Ieee80211FrequencyBand> frequencyBands) noexcept = 0;

    /**
     * @brief Set the authentication algorithms the access point should enable.
     *
     * @param authenticationAlgorithms The authentication algorithms to be allow.
     * @return AccessPointOperationStatus
     */
    virtual AccessPointOperationStatus
    SetAuthenticationAlgorithms(std::vector<Ieee80211AuthenticationAlgorithm> authenticationAlgorithms) noexcept = 0;

    /**
     * @brief Set the SSID of the access point.
     *
     * @param ssid The SSID to be set.
     * @return AccessPointOperationStatus
     */
    virtual AccessPointOperationStatus
    SetSsid(std::string_view ssid) noexcept = 0;
};

/**
 * @brief Factory for creating IAccessPointController objects.
 */
struct IAccessPointControllerFactory
{
    IAccessPointControllerFactory() = default;

    virtual ~IAccessPointControllerFactory() = default;

    /**
     * Prevent copying and moving of IAccessPointControllerFactory objects.
     */
    IAccessPointControllerFactory(const IAccessPointControllerFactory&) = delete;

    IAccessPointControllerFactory(IAccessPointControllerFactory&&) = delete;

    IAccessPointControllerFactory&
    operator=(const IAccessPointControllerFactory&) = delete;

    IAccessPointControllerFactory&
    operator=(IAccessPointControllerFactory&&) = delete;

    /**
     * @brief Create a new IAccessPointController object.
     *
     * @return std::unique_ptr<IAccessPointController>
     */
    virtual std::unique_ptr<IAccessPointController>
    Create(std::string_view interfaceName) = 0;
};
} // namespace Microsoft::Net::Wifi

#endif // I_ACCESS_POINT_CONTROLLER_HXX
