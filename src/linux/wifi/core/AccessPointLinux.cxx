
#include <microsoft/net/wifi/AccessPointLinux.hxx>

using namespace Microsoft::Net::Wifi;

std::unique_ptr<IAccessPointController>
AccessPointLinux::CreateController()
{
    return nullptr;
}
