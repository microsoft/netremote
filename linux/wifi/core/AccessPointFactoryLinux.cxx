
#include <microsoft/net/wifi/AccessPointLinux.hxx>
#include <microsoft/net/wifi/AccessPointFactoryLinux.hxx>

using namespace Microsoft::Net::Wifi;

std::shared_ptr<IAccessPoint> AccessPointFactoryLinux::Create(std::string_view interface)
{
    return std::make_shared<AccessPointLinux>(interface);
}
