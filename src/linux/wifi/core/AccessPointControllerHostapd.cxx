
#include <microsoft/net/wifi/AccessPointControllerHostapd.hxx>

using namespace Microsoft::Net::Wifi;

AccessPointControllerHostapd::AccessPointControllerHostapd(std::string_view interfaceName) :
    AccessPointController(interfaceName),
    m_wpaController(interfaceName, Wpa::WpaType::Hostapd)
{
}

AccessPointCapabilities
AccessPointControllerHostapd::GetCapabilities()
{
    // TODO: Implement this method.
    return {};
}
