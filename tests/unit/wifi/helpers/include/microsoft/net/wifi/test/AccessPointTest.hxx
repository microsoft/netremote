
#ifndef ACCESS_POINT_TEST_HXX
#define ACCESS_POINT_TEST_HXX

#include <memory>
#include <string_view>

#include <microsoft/net/wifi/IAccessPoint.hxx>

namespace Microsoft::Net::Wifi::Test
{
struct AccessPointTest final :
    public IAccessPoint
{
    AccessPointTest(std::string_view interfaceName);

    virtual std::string_view
    GetInterface() const noexcept override;

    /**
     * @brief Create a new instance that can control the access point.
     *
     * @return std::unique_ptr<IAccessPointController>
     */
    virtual std::unique_ptr<IAccessPointController>
    CreateController() override;

    std::string InterfaceName;
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
     * @param createArgs 
     *
     * @return std::shared_ptr<IAccessPoint>
     */
    std::shared_ptr<IAccessPoint>
    Create(std::string_view interfaceName, std::unique_ptr<IAccessPointCreateArgs> createArgs) override;
};
} // namespace Microsoft::Net::Wifi::Test

#endif // ACCESS_POINT_TEST_HXX
