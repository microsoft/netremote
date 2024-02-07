
#ifndef ACCESS_POINT_TEST_HXX
#define ACCESS_POINT_TEST_HXX

#include <memory>
#include <string_view>
#include <vector>

#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <microsoft/net/wifi/Ieee80211AccessPointCapabilities.hxx>

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
    std::string InterfaceName;
    Microsoft::Net::Wifi::Ieee80211AccessPointCapabilities Capabilities;
    bool IsEnabled{ false };
    Microsoft::Net::Wifi::Ieee80211Protocol Protocol;
    std::vector<Microsoft::Net::Wifi::Ieee80211FrequencyBand> FrequencyBands;

    /**
     * @brief Construct a new AccessPointTest object with the given interface name and default capabilities.
     *
     * @param interfaceName The interface name to use for the access point.
     */
    AccessPointTest(std::string_view interfaceName);

    /**
     * @brief Construct a new AccessPointTest object with the given interface name and capabilities.
     *
     * @param interfaceName The interface name to use for the access point.
     * @param capabilities The capabilities to use for the access point.
     */
    AccessPointTest(std::string_view interfaceName, Microsoft::Net::Wifi::Ieee80211AccessPointCapabilities capabilities);

    /**
     * @brief Get the interface name of the access point.
     *
     * @return std::string_view
     */
    virtual std::string_view
    GetInterfaceName() const override;

    /**
     * @brief Create a new instance that can control the access point.
     *
     * @return std::unique_ptr<IAccessPointController>
     */
    virtual std::unique_ptr<IAccessPointController>
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
    ~AccessPointFactoryTest() = default;

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
