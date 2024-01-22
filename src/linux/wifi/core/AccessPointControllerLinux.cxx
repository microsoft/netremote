
#include <format>

#include <microsoft/net/wifi/AccessPointControllerLinux.hxx>
#include <plog/Log.h>
#include <Wpa/IHostapd.hxx>
#include <Wpa/ProtocolHostapd.hxx>
#include <Wpa/WpaCommandStatus.hxx>
#include <Wpa/WpaResponseStatus.hxx>

using namespace Microsoft::Net::Wifi;

AccessPointControllerHostapd::AccessPointControllerHostapd(std::string_view interfaceName) :
    AccessPointController(interfaceName),
    m_hostapd(interfaceName)
{
}

Ieee80211AccessPointCapabilities
AccessPointControllerHostapd::GetCapabilities()
{
    // TODO: Implement this method.
    return {};
}

bool
AccessPointControllerHostapd::GetIsEnabled()
{
    bool isEnabled{ false };

    try {
        auto hostapdStatus = m_hostapd.GetStatus();
        isEnabled = (hostapdStatus.State == Wpa::HostapdInterfaceState::Enabled);
    } catch (const Wpa::HostapdException& ex) {
        throw AccessPointControllerException(std::format("Failed to get status for interface {} ({})", GetInterfaceName(), ex.what()));
    }

    return isEnabled;
}

std::unique_ptr<IAccessPointController>
AccessPointControllerHostapdFactory::Create(std::string_view interfaceName)
{
    return std::make_unique<AccessPointControllerHostapd>(interfaceName);
}
