
#include <microsoft/net/wifi/AccessPointController.hxx>

using namespace Microsoft::Net::Wifi;

AccessPointController::AccessPointController(std::string_view interface) :
    m_interface(interface)
{
}

std::string_view
AccessPointController::GetInterface() const noexcept
{
    return m_interface;
}
