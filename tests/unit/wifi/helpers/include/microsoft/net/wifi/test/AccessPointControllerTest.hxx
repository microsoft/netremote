
#ifndef ACCESS_POINT_CONTROLLER_FACTORY_TEST
#define ACCESS_POINT_CONTROLLER_FACTORY_TEST

#include <memory>
#include <string_view>

#include <microsoft/net/wifi/IAccessPointController.hxx>

namespace Microsoft::Net::Wifi::Test
{
struct AccessPointControllerTest final :
    public IAccessPointController
{
    AccessPointControllerTest(std::string_view interfaceName);

    virtual std::string_view
    GetInterfaceName() const noexcept override;

    virtual AccessPointCapabilities2
    GetCapabilities() override;

    std::string InterfaceName;
};

struct AccessPointControllerFactoryTest final :
    public Microsoft::Net::Wifi::IAccessPointControllerFactory
{
    std::unique_ptr<Microsoft::Net::Wifi::IAccessPointController>
    Create(std::string_view interfaceName) override;
};
} // namespace Microsoft::Net::Wifi::Test

#endif // ACCESS_POINT_CONTROLLER_FACTORY_TEST
