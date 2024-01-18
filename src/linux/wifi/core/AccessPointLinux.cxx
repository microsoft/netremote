
#include <microsoft/net/wifi/AccessPointLinux.hxx>

using namespace Microsoft::Net::Wifi;

std::unique_ptr<IAccessPointController>
AccessPointLinux::CreateController()
{
    return nullptr;
}

std::shared_ptr<IAccessPoint>
AccessPointFactoryLinux::Create(std::string_view interface)
{
    return std::make_shared<AccessPointLinux>(interface);
}
