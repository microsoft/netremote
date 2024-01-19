
#include <microsoft/net/wifi/AccessPointControllerHostapd.hxx>
#include <microsoft/net/wifi/AccessPointLinux.hxx>

using namespace Microsoft::Net::Wifi;

std::unique_ptr<IAccessPointController>
AccessPointLinux::CreateController()
{
    return std::make_unique<AccessPointControllerHostapd>(GetInterface());
}

std::shared_ptr<IAccessPoint>
AccessPointFactoryLinux::Create(std::string_view interface)
{
    return std::make_shared<AccessPointLinux>(interface);
}
