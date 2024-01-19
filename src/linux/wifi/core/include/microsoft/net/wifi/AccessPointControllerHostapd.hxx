
#ifndef ACCESS_POINT_CONTROLLER_HOSTAPD_HXX
#define ACCESS_POINT_CONTROLLER_HOSTAPD_HXX

#include <microsoft/net/wifi/AccessPointController.hxx>

namespace Microsoft::Net::Wifi
{   
struct AccessPointControllerHostapd :
    public AccessPointController
{
    virtual ~AccessPointControllerHostapd() = default;

    using AccessPointController::AccessPointController;

    AccessPointControllerHostapd() = delete;
};
} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_CONTROLLER_HOSTAPD_HXX
