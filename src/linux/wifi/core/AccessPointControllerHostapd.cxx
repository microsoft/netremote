
#include <microsoft/net/wifi/AccessPointControllerHostapd.hxx>
#include <Wpa/WpaCommandGet.hxx>

using namespace Microsoft::Net::Wifi;

AccessPointControllerHostapd::AccessPointControllerHostapd(std::string_view interfaceName) :
    AccessPointController(interfaceName),
    m_wpaController(interfaceName, Wpa::WpaType::Hostapd)
{
}

AccessPointCapabilities2
AccessPointControllerHostapd::GetCapabilities()
{
    // TODO: Implement this method.
    return {};
}

std::unique_ptr<IAccessPointController>
AccessPointControllerHostapdFactory::Create(std::string_view interfaceName)
{
    return std::make_unique<AccessPointControllerHostapd>(interfaceName);
}
