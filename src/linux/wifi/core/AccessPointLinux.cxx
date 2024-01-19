
#include <microsoft/net/wifi/AccessPointControllerHostapd.hxx>
#include <microsoft/net/wifi/AccessPointLinux.hxx>

using namespace Microsoft::Net::Wifi;

std::unique_ptr<IAccessPointController>
AccessPointLinux::CreateController()
{
    return std::make_unique<AccessPointControllerHostapd>(GetInterface());
}

std::shared_ptr<IAccessPoint>
AccessPointFactoryLinux::Create(std::string_view interfaceName)
{
    return Create(interfaceName, nullptr);
}

std::shared_ptr<IAccessPoint>
AccessPointFactoryLinux::Create(std::string_view interfaceName, [[maybe_unused]] std::unique_ptr<IAccessPointCreateArgs> createArgs)
{
    return std::make_shared<AccessPointLinux>(interfaceName, GetControllerFactory());
}
