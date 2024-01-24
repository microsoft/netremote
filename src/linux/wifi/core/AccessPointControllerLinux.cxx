
#include <algorithm>
#include <format>
#include <ranges>

#include <Wpa/IHostapd.hxx>
#include <Wpa/ProtocolHostapd.hxx>
#include <Wpa/WpaCommandStatus.hxx>
#include <Wpa/WpaResponseStatus.hxx>
#include <microsoft/net/netlink/nl80211/Netlink80211Wiphy.hxx>
#include <microsoft/net/wifi/AccessPointControllerLinux.hxx>

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

Ieee80211AkmSuite
Nl80211AkmSuiteToIeee80211AkmSuite(uint32_t nl80211AkmSuite) noexcept
{
    return static_cast<Ieee80211AkmSuite>(nl80211AkmSuite);
}

Ieee80211FrequencyBand
Nl80211BandToIeee80211FrequencyBand(nl80211_band nl80211Band) noexcept
{
    switch (nl80211Band) {
    case NL80211_BAND_2GHZ:
        return Ieee80211FrequencyBand::TwoPointFourGHz;
    case NL80211_BAND_5GHZ:
        return Ieee80211FrequencyBand::FiveGHz;
    case NL80211_BAND_60GHZ:
        return Ieee80211FrequencyBand::SixGHz;
    default:
        return Ieee80211FrequencyBand::Unknown;
    }
}

std::vector<Ieee80211Protocol>
Nl80211WiphyToIeee80211Protocols(const Nl80211Wiphy& nl80211Wiphy)
{
    // Ieee80211 B & G are always supported.
    std::vector<Ieee80211Protocol> protocols{
        Ieee80211Protocol::B,
        Ieee80211Protocol::G,
    };

    for (const auto& band : std::views::values(nl80211Wiphy.Bands)) {
        if (band.HtCapabilities != 0) {
            protocols.push_back(Ieee80211Protocol::N);
        }
        if (band.VhtCapabilities != 0) {
            protocols.push_back(Ieee80211Protocol::AC);
        }
        // TODO: once Nl80211WiphyBand is updated to support HE (AX) and EHT (BE), add them here.
    }

    // Remove duplicates.
    std::ranges::sort(protocols);
    protocols.erase(std::ranges::begin(std::ranges::unique(protocols)), std::ranges::end(protocols));

    return protocols;
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

    // Convert protocols.
    capabilities.Protocols = detail::Nl80211WiphyToIeee80211Protocols(wiphy.value());

    // Convert frequency bands.
    capabilities.FrequencyBands = std::vector<Ieee80211FrequencyBand>(std::size(wiphy->Bands));
    std::ranges::transform(std::views::keys(wiphy->Bands), std::begin(capabilities.FrequencyBands), detail::Nl80211BandToIeee80211FrequencyBand);

    // Convert AKM suites.
    capabilities.AkmSuites = std::vector<Ieee80211AkmSuite>(std::size(wiphy->AkmSuites));
    std::ranges::transform(wiphy->AkmSuites, std::begin(capabilities.AkmSuites), detail::Nl80211AkmSuiteToIeee80211AkmSuite);

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
