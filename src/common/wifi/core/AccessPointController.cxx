
#include <microsoft/net/wifi/AccessPointController.hxx>

using namespace Microsoft::Net::Wifi;

AccessPointController::AccessPointController(std::weak_ptr<IAccessPoint> accessPointWeak) :
    m_accessPointWeak(accessPointWeak)
{}

std::weak_ptr<IAccessPoint>
AccessPointController::GetAccessPoint() const noexcept
{
    return m_accessPointWeak;
}
