
#include <string_view>

#include <microsoft/net/wifi/AccessPointController.hxx>

using namespace Microsoft::Net::Wifi;

AccessPointController::AccessPointController(std::string_view interface) :
    m_interfaceName(interface)
{
}

std::string_view
AccessPointController::GetInterfaceName() const
{
    return m_interfaceName;
}
