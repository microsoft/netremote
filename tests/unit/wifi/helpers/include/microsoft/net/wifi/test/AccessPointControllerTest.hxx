
#ifndef ACCESS_POINT_CONTROLLER_FACTORY_TEST
#define ACCESS_POINT_CONTROLLER_FACTORY_TEST

#include <memory>
#include <string_view>

#include <microsoft/net/wifi/IAccessPointController.hxx>

namespace Microsoft::Net::Wifi::Test
{
struct AccessPointTest;

/**
 * @brief IAccessPointController implementation for testing purposes.
 *
 * This implementation takes an AccessPointTest object which it uses to implement the IAccessPointController interface.
 * The owner of this class must ensure that the passes AccessPointTest* remains valid for the lifetime of this object.
 */
struct AccessPointControllerTest final :
    public Microsoft::Net::Wifi::IAccessPointController
{
    AccessPointTest *AccessPoint{ nullptr };

    /**
     * @brief Construct a new AccessPointControllerTest object that uses the specified AccessPointTest to implement the
     * IAccessPointController interface.
     *
     * @param accessPoint The access point to use.
     */
    AccessPointControllerTest(AccessPointTest *accessPoint);

    /**
     * @brief Get the interface name associated with this controller.
     *
     * @return std::string_view
     */
    virtual std::string_view
    GetInterfaceName() const override;

    /**
     * @brief Get whether the access point is enabled.
     *
     * @return true
     * @return false
     */
    virtual bool
    GetIsEnabled() override;

    /**
     * @brief Get the capabilities of the access point.
     *
     * @return Ieee80211AccessPointCapabilities
     */
    virtual Ieee80211AccessPointCapabilities
    GetCapabilities() override;

    /**
     * @brief Set the Ieee80211 protocol of the access point.
     * 
     * @param ieeeProtocol The Ieee80211 protocol to be set
     * @return true
     * @return false
    */
    virtual bool
    SetProtocol(Microsoft::Net::Wifi::Ieee80211Protocol ieeeProtocol) override;

    /**
     * @brief Set the AKM suites used by the access point.
     *
     * @param akmSuites The AKM suites to be set.
     * @return true
     * @return false
     */
    virtual bool
    SetAkmSuites(std::vector<Microsoft::Net::Wifi::Ieee80211AkmSuite> akmSuites) override;

    /**
     * @brief Set the cipher suites used by the access point.
     *
     * @param cipherSuites The cipher suites to be set.
     * @return true
     * @return false
     */
    virtual bool
    SetCipherSuites(std::vector<Microsoft::Net::Wifi::Ieee80211CipherSuite> cipherSuites) override;
};

/**
 * @brief IAccessPointControllerFactory implementation for testing purposes.
 */
struct AccessPointControllerFactoryTest final :
    public Microsoft::Net::Wifi::IAccessPointControllerFactory
{
    AccessPointTest *AccessPoint{ nullptr };

    /**
     * @brief Construct a new AccessPointControllerFactoryTest object with no AccessPointTest parent/owner. This may
     * only be used for cases where the constructed object won't actually be used (eg. unit tests for other components
     * that require an IAccessPointControllerFactory, but the test doesn't trigger any functionality to use it).
     *
     * This should be contrained to testing creation/destruction of other objects that use this only.
     */
    AccessPointControllerFactoryTest() = default;

    /**
     * @brief Construct a new AccessPointControllerFactoryTest object. The owner of this object must ensure that the passed AccessPointTest remains valid for the lifetime of this object.
     *
     * @param accessPoint The access point to create controllers for.
     */
    AccessPointControllerFactoryTest(AccessPointTest *accessPoint);

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
