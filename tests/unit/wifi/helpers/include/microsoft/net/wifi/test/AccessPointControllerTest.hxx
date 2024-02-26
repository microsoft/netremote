
#ifndef ACCESS_POINT_CONTROLLER_FACTORY_TEST
#define ACCESS_POINT_CONTROLLER_FACTORY_TEST

#include <memory>
#include <string_view>
#include <vector>

#include <microsoft/net/wifi/AccessPointOperationStatus.hxx>
#include <microsoft/net/wifi/IAccessPointController.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <microsoft/net/wifi/Ieee80211AccessPointCapabilities.hxx>

namespace Microsoft::Net::Wifi::Test
{
struct AccessPointTest;

/**
 * @brief IAccessPointController implementation for testing purposes.
 *
 * This implementation takes an AccessPointTest object which it uses to implement the IAccessPointController interface.
 * The owner of this class must ensure that the passed AccessPointTest* remains valid for the lifetime of this object.
 */
struct AccessPointControllerTest final :
    public Microsoft::Net::Wifi::IAccessPointController
{
    AccessPointTest *AccessPoint{ nullptr };

    AccessPointControllerTest() = default;
    ~AccessPointControllerTest() override = default;

    /**
     * @brief Construct a new AccessPointControllerTest object that uses the specified AccessPointTest to implement the
     * IAccessPointController interface.
     *
     * @param accessPoint The access point to use.
     */
    explicit AccessPointControllerTest(AccessPointTest *accessPoint);

    /**
     * Prevent copying and moving of AccessPointControllerTest objects.
     */
    AccessPointControllerTest(const AccessPointControllerTest &) = delete;

    AccessPointControllerTest(AccessPointControllerTest &&) = delete;

    AccessPointControllerTest &
    operator=(const AccessPointControllerTest &) = delete;

    AccessPointControllerTest &
    operator=(AccessPointControllerTest &&) = delete;

    /**
     * @brief Get the interface name associated with this controller.
     *
     * @return std::string_view
     */
    std::string_view
    GetInterfaceName() const override;

    /**
     * @brief Get the access point operational state.
     *
     * @param operationalState The value to store the operational state.
     * @return AccessPointOperationStatus
     */
    AccessPointOperationStatus
    GetOperationalState(AccessPointOperationalState &operationalState) override;

    /**
     * @brief Get the capabilities of the access point.
     *
     * @param ieee80211AccessPointCapabilities The value to store the capabilities.
     * @return AccessPointOperationStatus
     */
    AccessPointOperationStatus
    GetCapabilities(Ieee80211AccessPointCapabilities &ieee80211AccessPointCapabilities) override;

    /**
     * @brief Set the operational state of the access point.
     *
     * @param operationalState The desired operational state.
     * @return AccessPointOperationStatus
     */
    AccessPointOperationStatus
    SetOperationalState(AccessPointOperationalState operationalState) override;

    /**
     * @brief Set the Ieee80211 protocol of the access point.
     *
     * @param ieeeProtocol The Ieee80211 protocol to be set
     * @return AccessPointOperationStatus
     */
    AccessPointOperationStatus
    SetProtocol(Microsoft::Net::Wifi::Ieee80211Protocol ieeeProtocol) override;

    /**
     * @brief Set the frquency bands the access point should enable.
     *
     * @param frequencyBands The frequency bands to be set.
     * @return AccessPointOperationStatus
     */
    AccessPointOperationStatus
    SetFrequencyBands(std::vector<Microsoft::Net::Wifi::Ieee80211FrequencyBand> frequencyBands) override;

    /**
     * @brief Set the SSID of the access point.
     *
     * @param ssid The SSID to be set.
     * @return AccessPointOperationStatus
     */
    AccessPointOperationStatus
    SetSssid(std::string_view ssid) override;
};

/**
 * @brief IAccessPointControllerFactory implementation for testing purposes.
 */
struct AccessPointControllerFactoryTest final : public Microsoft::Net::Wifi::IAccessPointControllerFactory
{
    AccessPointTest *AccessPoint{ nullptr };

    ~AccessPointControllerFactoryTest() override = default;

    /**
     * @brief Construct a new AccessPointControllerFactoryTest object with no AccessPointTest parent/owner. This may
     * only be used for cases where the constructed object won't actually be used (eg. unit tests for other components
     * that require an IAccessPointControllerFactory, but the test doesn't trigger any functionality to use it).
     *
     * This should be contrained to testing creation/destruction of other objects that use this only.
     */
    AccessPointControllerFactoryTest() = default;

    /**
     * @brief Construct a new AccessPointControllerFactoryTest object. The owner of this object must ensure that the
     * passed AccessPointTest remains valid for the lifetime of this object.
     *
     * @param accessPoint The access point to create controllers for.
     */
    explicit AccessPointControllerFactoryTest(AccessPointTest *accessPoint);

    /**
     * Prevent copying and moving of AccessPointControllerFactoryTest objects.
     */
    AccessPointControllerFactoryTest(const AccessPointControllerFactoryTest &) = delete;
    AccessPointControllerFactoryTest &
    operator=(const AccessPointControllerFactoryTest &) = delete;
    AccessPointControllerFactoryTest(AccessPointControllerFactoryTest &&) = delete;
    AccessPointControllerFactoryTest &
    operator=(AccessPointControllerFactoryTest &&) = delete;

    /**
     * @brief Create a new instance that can control the access point.
     *
     * @param interfaceName The name of the interface. If this factory was created with an AccessPointTest, this must
     * match the interface name of the AccessPoint the object or else a std::runtime_error will be thrown.  was created
     * @return std::unique_ptr<Microsoft::Net::Wifi::IAccessPointController>
     */
    std::unique_ptr<Microsoft::Net::Wifi::IAccessPointController>
    Create(std::string_view interfaceName) override;
};
} // namespace Microsoft::Net::Wifi::Test

#endif // ACCESS_POINT_CONTROLLER_FACTORY_TEST
