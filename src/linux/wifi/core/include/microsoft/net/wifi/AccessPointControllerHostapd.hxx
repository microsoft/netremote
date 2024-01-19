
#ifndef ACCESS_POINT_CONTROLLER_HOSTAPD_HXX
#define ACCESS_POINT_CONTROLLER_HOSTAPD_HXX

#include <string_view>

#include <Wpa/WpaController.hxx>
#include <microsoft/net/wifi/AccessPointController.hxx>

namespace Microsoft::Net::Wifi
{
struct AccessPointControllerHostapd :
    public AccessPointController
{
    virtual ~AccessPointControllerHostapd() = default;

    AccessPointControllerHostapd() = delete;

    /**
     * @brief Construct a new AccessPointControllerHostapd object for the specified interface.
     * 
     * @param interfaceName The name of the interface to control.
     */
    AccessPointControllerHostapd(std::string_view interfaceName);

    virtual AccessPointCapabilities
    GetCapabilities() override;

private:
    Wpa::WpaController m_wpaController;
};
} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_CONTROLLER_HOSTAPD_HXX
