
#include <algorithm>
#include <format>
#include <ranges>

#include <microsoft/net/wifi/AccessPointControllerLinux.hxx>
#include <microsoft/net/netlink/nl80211/Netlink80211Wiphy.hxx>
#include <plog/Log.h>
#include <Wpa/IHostapd.hxx>
#include <Wpa/ProtocolHostapd.hxx>
#include <Wpa/WpaCommandStatus.hxx>
#include <Wpa/WpaResponseStatus.hxx>

using namespace Microsoft::Net::Wifi;

using Microsoft::Net::Netlink::Nl80211::Nl80211Wiphy;

AccessPointControllerLinux::AccessPointControllerLinux(std::string_view interfaceName) :
    AccessPointController(interfaceName),
    m_hostapd(interfaceName)
{
}

namespace detail
{
Ieee80211CipherSuite
Nl80211CipherSuiteToIeee80211CipherSuite(uint32_t nl80211CipherSuite) noexcept
{
    return static_cast<Ieee80211CipherSuite>(nl80211CipherSuite);
}
} // namespace detail

Ieee80211AccessPointCapabilities
AccessPointControllerLinux::GetCapabilities()
{
    auto wiphy = Nl80211Wiphy::FromInterfaceName(GetInterfaceName());
    if (!wiphy.has_value()) {
        throw AccessPointControllerException(std::format("Failed to get wiphy for interface {}", GetInterfaceName()));
    }

    Ieee80211AccessPointCapabilities capabilities;

    // Convert cipher suites.
    capabilities.CipherSuites = std::vector<Ieee80211CipherSuite>(std::size(wiphy->CipherSuites));
    std::ranges::transform(wiphy->CipherSuites, std::begin(capabilities.CipherSuites), detail::Nl80211CipherSuiteToIeee80211CipherSuite);

    return capabilities;
}

bool
AccessPointControllerLinux::GetIsEnabled()
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
AccessPointControllerLinuxFactory::Create(std::string_view interfaceName)
{
    return std::make_unique<AccessPointControllerLinux>(interfaceName);
}
