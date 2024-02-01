
#include <algorithm>
#include <format>
#include <ranges>

#include <Wpa/IHostapd.hxx>
#include <Wpa/ProtocolHostapd.hxx>
#include <Wpa/WpaCommandStatus.hxx>
#include <Wpa/WpaResponseStatus.hxx>
#include <magic_enum.hpp>
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

Wpa::HostapdHwMode
IeeeProtocolToHostapdHwMode(Ieee80211Protocol ieeeProtocol)
{
    switch (ieeeProtocol) {
    case Ieee80211Protocol::B:
        return Wpa::HostapdHwMode::Ieee80211b;
    case Ieee80211Protocol::G:
        return Wpa::HostapdHwMode::Ieee80211g;
    case Ieee80211Protocol::N:
        return Wpa::HostapdHwMode::Ieee80211a; // TODO: Could be a or g depending on band
    case Ieee80211Protocol::A:
        return Wpa::HostapdHwMode::Ieee80211a;
    case Ieee80211Protocol::AC:
        return Wpa::HostapdHwMode::Ieee80211a;
    case Ieee80211Protocol::AD:
        return Wpa::HostapdHwMode::Ieee80211ad;
    case Ieee80211Protocol::AX:
        return Wpa::HostapdHwMode::Ieee80211a;
    case Ieee80211Protocol::BE:
        return Wpa::HostapdHwMode::Ieee80211a; // TODO: Assuming a, although hostapd docs don't mention it
    default:
        return Wpa::HostapdHwMode::Unknown;
    }
}

std::string_view
HostapdHwModeToPropertyValue(Wpa::HostapdHwMode hwMode)
{
    switch (hwMode) {
    case Wpa::HostapdHwMode::Ieee80211b:
        return Wpa::ProtocolHostapd::PropertyHwModeValueB;
    case Wpa::HostapdHwMode::Ieee80211g:
        return Wpa::ProtocolHostapd::PropertyHwModeValueG;
    case Wpa::HostapdHwMode::Ieee80211a:
        return Wpa::ProtocolHostapd::PropertyHwModeValueA;
    case Wpa::HostapdHwMode::Ieee80211ad:
        return Wpa::ProtocolHostapd::PropertyHwModeValueAD;
    case Wpa::HostapdHwMode::Ieee80211any:
        return Wpa::ProtocolHostapd::PropertyHwModeValueAny;
    default: // case Wpa::HostapdHwMode::Unknown
        throw AccessPointControllerException(std::format("Invalid hostapd hw_mode value {}", magic_enum::enum_name(hwMode)));
    }
}

std::string_view
IeeeAkmSuiteToWpaKeyMgmtPropertyValue(Ieee80211AkmSuite akmSuite)
{
    switch (akmSuite) {
    case Ieee80211AkmSuite::Ieee8021x:
        return Wpa::ProtocolHostapd::PropertyWpaKeyMgmtValueWpaEap;
    case Ieee80211AkmSuite::Psk:
        return Wpa::ProtocolHostapd::PropertyWpaKeyMgmtValueWpaPsk;
    case Ieee80211AkmSuite::Ft8021x:
        return Wpa::ProtocolHostapd::PropertyWpaKeyMgmtValueFtEap;
    case Ieee80211AkmSuite::FtPsk:
        return Wpa::ProtocolHostapd::PropertyWpaKeyMgmtValueFtPsk;
    case Ieee80211AkmSuite::Ieee8021xSha256:
        return Wpa::ProtocolHostapd::PropertyWpaKeyMgmtValueWpaEapSha256;
    case Ieee80211AkmSuite::PskSha256:
        return Wpa::ProtocolHostapd::PropertyWpaKeyMgmtValueWpaPskSha256;
    case Ieee80211AkmSuite::Sae:
        return Wpa::ProtocolHostapd::PropertyWpaKeyMgmtValueSae;
    case Ieee80211AkmSuite::FtSae:
        return Wpa::ProtocolHostapd::PropertyWpaKeyMgmtValueFtSae;
    case Ieee80211AkmSuite::Ieee8021xSuiteB:
        return Wpa::ProtocolHostapd::PropertyWpaKeyMgmtValueWpaEapSuiteB;
    case Ieee80211AkmSuite::Ieee8021xSuiteB192:
        return Wpa::ProtocolHostapd::PropertyWpaKeyMgmtValueWpaEapSuiteB192;
    case Ieee80211AkmSuite::Ft8021xSha384:
        return Wpa::ProtocolHostapd::PropertyWpaKeyMgmtValueFtEapSha384;
    case Ieee80211AkmSuite::FilsSha256:
        return Wpa::ProtocolHostapd::PropertyWpaKeyMgmtValueFilsSha256;
    case Ieee80211AkmSuite::FilsSha384:
        return Wpa::ProtocolHostapd::PropertyWpaKeyMgmtValueFilsSha384;
    case Ieee80211AkmSuite::FtFilsSha256:
        return Wpa::ProtocolHostapd::PropertyWpaKeyMgmtValueFtFilsSha256;
    case Ieee80211AkmSuite::FtFilsSha384:
        return Wpa::ProtocolHostapd::PropertyWpaKeyMgmtValueFtFilsSha384;
    case Ieee80211AkmSuite::Owe:
        return Wpa::ProtocolHostapd::PropertyWpaKeyMgmtValueOwe;
    case Ieee80211AkmSuite::Reserved0:
        [[fallthrough]];
    case Ieee80211AkmSuite::Tdls:
        [[fallthrough]];
    case Ieee80211AkmSuite::ApPeerKey:
        [[fallthrough]];
    case Ieee80211AkmSuite::FtPskSha384:
        [[fallthrough]];
    case Ieee80211AkmSuite::PskSha384:
        [[fallthrough]];
    default:
        throw AccessPointControllerException(std::format("Invalid Ieee80211AkmSuite value {} for hostapd wpa_key_mgmt", magic_enum::enum_name(akmSuite)));
    }
}

std::string_view
IeeeCipherSuiteToPairwisePropertyValue(Ieee80211CipherSuite cipherSuite)
{
    switch (cipherSuite) {
    case Ieee80211CipherSuite::Unknown:
        return "";
    case Ieee80211CipherSuite::BipCmac128:
        return "";
    case Ieee80211CipherSuite::BipCmac256:
        return "";
    case Ieee80211CipherSuite::BipGmac128:
        return "";
    case Ieee80211CipherSuite::BipGmac256:
        return "";
    case Ieee80211CipherSuite::Ccmp128:
        return Wpa::ProtocolHostapd::PropertyPairwiseValueCcmp;
    case Ieee80211CipherSuite::Ccmp256:
        return Wpa::ProtocolHostapd::PropertyPairwiseValueCcmp256;
    case Ieee80211CipherSuite::Gcmp128:
        return Wpa::ProtocolHostapd::PropertyPairwiseValueGcmp;
    case Ieee80211CipherSuite::Gcmp256:
        return Wpa::ProtocolHostapd::PropertyPairwiseValueGcmp256;
    case Ieee80211CipherSuite::GroupAddressesTrafficNotAllowed:
        return "";
    case Ieee80211CipherSuite::Tkip:
        return Wpa::ProtocolHostapd::PropertyPairwiseValueTkip;
    case Ieee80211CipherSuite::UseGroup:
        return "";
    case Ieee80211CipherSuite::Wep104:
        return "";
    case Ieee80211CipherSuite::Wep40:
        return "";
    default:
        return "";
    }
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

bool
AccessPointControllerLinux::SetProtocol(Ieee80211Protocol ieeeProtocol)
{
    bool isOk = false;
    Wpa::HostapdHwMode hwMode = detail::IeeeProtocolToHostapdHwMode(ieeeProtocol);

    try {
        // Set the hostapd hw_mode property.
        isOk = m_hostapd.SetProperty(Wpa::ProtocolHostapd::PropertyNameHwMode, detail::HostapdHwModeToPropertyValue(hwMode));

        // Additively set other hostapd properties based on the protocol.
        switch (ieeeProtocol) {
        case Ieee80211Protocol::AX:
            isOk = isOk && m_hostapd.SetProperty(Wpa::ProtocolHostapd::PropertyNameIeee80211AX, Wpa::ProtocolHostapd::PropertyEnabled);
            isOk = isOk && m_hostapd.SetProperty(Wpa::ProtocolHostapd::PropertyNameDisable11AX, Wpa::ProtocolHostapd::PropertyDisabled);
            [[fallthrough]];
        case Ieee80211Protocol::AC:
            isOk = isOk && m_hostapd.SetProperty(Wpa::ProtocolHostapd::PropertyNameIeee80211AC, Wpa::ProtocolHostapd::PropertyEnabled);
            isOk = isOk && m_hostapd.SetProperty(Wpa::ProtocolHostapd::PropertyNameDisable11AC, Wpa::ProtocolHostapd::PropertyDisabled);
            [[fallthrough]];
        case Ieee80211Protocol::N:
            isOk = isOk && m_hostapd.SetProperty(Wpa::ProtocolHostapd::PropertyNameWmmEnabled, Wpa::ProtocolHostapd::PropertyEnabled);
            isOk = isOk && m_hostapd.SetProperty(Wpa::ProtocolHostapd::PropertyNameIeee80211N, Wpa::ProtocolHostapd::PropertyEnabled);
            isOk = isOk && m_hostapd.SetProperty(Wpa::ProtocolHostapd::PropertyNameDisable11N, Wpa::ProtocolHostapd::PropertyDisabled);
            break;
        default:
            break;
        }
    } catch (const Wpa::HostapdException& ex) {
        throw AccessPointControllerException(std::format("Failed to set Ieee80211 protocol for interface {} ({})", GetInterfaceName(), ex.what()));
    }

    // Reload hostapd conf file.
    return isOk && m_hostapd.Reload();
}

bool
AccessPointControllerLinux::SetAkmSuites(std::vector<Ieee80211AkmSuite> akmSuites)
{
    bool isOk = false;

    // Construct the wpa_key_mgmt hostapd property value.
    std::string concatenatedWpaKeyMgmtPropertyValue{};
    for (const auto& akmSuite : akmSuites) {
        const auto& wpaKeyMgmtPropertyValue = detail::IeeeAkmSuiteToWpaKeyMgmtPropertyValue(akmSuite);
        concatenatedWpaKeyMgmtPropertyValue += wpaKeyMgmtPropertyValue;
        concatenatedWpaKeyMgmtPropertyValue += " ";
    }

    try {
        // Set the hostapd wpa_key_mgmt property.
        isOk = m_hostapd.SetProperty(Wpa::ProtocolHostapd::PropertyNameWpaKeyMgmt, concatenatedWpaKeyMgmtPropertyValue);

        // For simplicity, set the hostapd wpa property to allow WPA and WPA2 (and WPA3).
        isOk = isOk && m_hostapd.SetProperty(Wpa::ProtocolHostapd::PropertyNameWpa, Wpa::ProtocolHostapd::PropertyWpaValueWpaAndWpa2);

        // For simplicity, set the hostapd auth_algs property to allow both Open System Authentication and Shared Key Authentication.
        isOk = isOk && m_hostapd.SetProperty(Wpa::ProtocolHostapd::PropertyNameAuthAlgs, Wpa::ProtocolHostapd::PropertyAuthAlgsValueOpenAndSharedKey);
    } catch (const Wpa::HostapdException& ex) {
        throw AccessPointControllerException(std::format("Failed to set AKM suites for interface {} ({})", GetInterfaceName(), ex.what()));
    }

    // Reload hostapd conf file.
    return isOk && m_hostapd.Reload();
}

bool
AccessPointControllerLinux::SetCipherSuites(std::vector<Ieee80211CipherSuite> cipherSuites)
{
    bool isOk = false;

    // For simplicity, construct both wpa_pairwise and rsn_pairwise hostapd property values.
    std::string concatenatedPairwisePropertyValue{};
    for (const auto& cipherSuite : cipherSuites) {
        const auto& pairwisePropertyValue = detail::IeeeCipherSuiteToPairwisePropertyValue(cipherSuite);
        concatenatedPairwisePropertyValue += pairwisePropertyValue;
        concatenatedPairwisePropertyValue += " ";
    }

    try {
        // Set the hostapd wpa_pairwise property.
        isOk = m_hostapd.SetProperty(Wpa::ProtocolHostapd::PropertyNameWpaPairwise, concatenatedPairwisePropertyValue);

        // Set the hostapd rsn_pairwise property.
        isOk = isOk && m_hostapd.SetProperty(Wpa::ProtocolHostapd::PropertynameRsnPairwise, concatenatedPairwisePropertyValue);
    } catch (const Wpa::HostapdException& ex) {
        throw AccessPointControllerException(std::format("Failed to set cipher suites for interface {} ({})", GetInterfaceName(), ex.what()));
    }

    // Reload hostapd conf file.
    return isOk && m_hostapd.Reload();
}

std::unique_ptr<IAccessPointController>
AccessPointControllerLinuxFactory::Create(std::string_view interfaceName)
{
    return std::make_unique<AccessPointControllerLinux>(interfaceName);
}
