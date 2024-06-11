
#ifndef ACCESS_POINT_TEST_HXX
#define ACCESS_POINT_TEST_HXX

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <microsoft/net/Ieee8021xRadiusAuthentication.hxx>
#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPointController.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <microsoft/net/wifi/Ieee80211AccessPointCapabilities.hxx>
#include <microsoft/net/wifi/Ieee80211Authentication.hxx>

namespace Microsoft::Net::Wifi::Test
{
/**
 * @brief IAccessPoint implementation for testing purposes.
 *
 * This implementation has public members for each configurable setting, allowing consumers to read and change them at
 * will. This is intended to be used by test code only.
 */
struct AccessPointTest final :
    public IAccessPoint
{
    std::string Ssid;
    std::string InterfaceName;
    std::string BridgeInterfaceId;
    Microsoft::Net::Wifi::Ieee80211MacAddress MacAddress{};
    Microsoft::Net::Wifi::Ieee80211AccessPointCapabilities Capabilities;
    Microsoft::Net::Wifi::Ieee80211PhyType PhyType{ Microsoft::Net::Wifi::Ieee80211PhyType::Unknown };
    Microsoft::Net::Wifi::Ieee80211Authentication8021x Authentication8021x;
    Microsoft::Net::Wifi::Ieee80211AuthenticationData AuthenticationData;
    std::vector<Microsoft::Net::Wifi::Ieee80211FrequencyBand> FrequencyBands;
    std::vector<Microsoft::Net::Wifi::Ieee80211AuthenticationAlgorithm> AuthenticationAlgorithms;
    std::vector<Microsoft::Net::Wifi::Ieee80211AkmSuite> AkmSuites;
    std::unordered_map<Ieee80211SecurityProtocol, std::vector<Ieee80211CipherSuite>> CipherSuites;
    AccessPointOperationalState OperationalState{ AccessPointOperationalState::Disabled };

    /**
     * @brief Construct a new AccessPointTest object with the given interface name and default capabilities.
     *
     * @param interfaceName The interface name to use for the access point.
     */
    explicit AccessPointTest(std::string_view interfaceName);

    /**
     * @brief Construct a new AccessPointTest object with the given interface name and capabilities.
     *
     * @param interfaceName The interface name to use for the access point.
     * @param capabilities The capabilities to use for the access point.
     */
    AccessPointTest(std::string_view interfaceName, Microsoft::Net::Wifi::Ieee80211AccessPointCapabilities capabilities);

    AccessPointTest() = delete;
    ~AccessPointTest() override = default;

    /**
     * Prevent copying and moving of this object.
     */
    AccessPointTest(const AccessPointTest&) = delete;

    AccessPointTest(AccessPointTest&&) = delete;

    AccessPointTest&
    operator=(const AccessPointTest&) = delete;

    AccessPointTest&
    operator=(AccessPointTest&&) = delete;

    /**
     * @brief Get the interface name of the access point.
     *
     * @return std::string_view
     */
    std::string_view
    GetInterfaceName() const noexcept override;

    /**
     * @brief Get the mac address of the access point.
     *
     * @return Microsoft::Net::Wifi::Ieee80211MacAddress
     */
    Microsoft::Net::Wifi::Ieee80211MacAddress
    GetMacAddress() const noexcept override;

    /**
     * @brief Create a new instance that can control the access point.
     *
     * @return std::unique_ptr<IAccessPointController>
     */
    std::unique_ptr<IAccessPointController>
    CreateController() override;
};

/**
 * @brief Basic factory implementation to use for testing purposes. This
 * implementation creates AccessPoints that perform no-ops.
 */
struct AccessPointFactoryTest :
    public Microsoft::Net::Wifi::IAccessPointFactory
{
    AccessPointFactoryTest() = default;
    ~AccessPointFactoryTest() override = default;

    /**
     * Prevent copying and moving of this object.
     */
    AccessPointFactoryTest(const AccessPointFactoryTest&) = delete;
    AccessPointFactoryTest&
    operator=(const AccessPointFactoryTest&) = delete;
    AccessPointFactoryTest(AccessPointFactoryTest&&) = delete;
    AccessPointFactoryTest&
    operator=(AccessPointFactoryTest&&) = delete;

    /**
     * @brief Create an AccessPoint for testing purposes.
     *
     * @param interface The interface to create the AccessPoint for. This can be
     * any string and does not have to correspond to a real device interface.
     *
     * @return std::shared_ptr<IAccessPoint>
     */
    std::shared_ptr<IAccessPoint>
    Create(std::string_view interface) override;

    /**
     * @brief Create an AccessPoint for testing purposes.
     *
     * @param interface The interface to create the AccessPoint for. This can be
     * any string and does not have to correspond to a real device interface.
     * @param createArgs Arguments to be passed to the access point during creation.
     *
     * @return std::shared_ptr<IAccessPoint>
     */
    std::shared_ptr<IAccessPoint>
    Create(std::string_view interfaceName, std::unique_ptr<IAccessPointCreateArgs> createArgs) override;
};
} // namespace Microsoft::Net::Wifi::Test

#endif // ACCESS_POINT_TEST_HXX
