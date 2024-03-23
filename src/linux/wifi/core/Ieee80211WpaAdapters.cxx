
#include <format>
#include <string>
#include <string_view>

#include <Wpa/ProtocolHostapd.hxx>
#include <magic_enum.hpp>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <plog/Log.h>

#include "Ieee80211WpaAdapters.hxx"

namespace Microsoft::Net::Wifi
{
using namespace Wpa;

HostapdHwMode
IeeePhyTypeToHostapdHwMode(Ieee80211PhyType ieeePhyType) noexcept
{
    switch (ieeePhyType) {
    case Ieee80211PhyType::B:
        return HostapdHwMode::Ieee80211b;
    case Ieee80211PhyType::G:
        return HostapdHwMode::Ieee80211g;
    case Ieee80211PhyType::N:
        return HostapdHwMode::Ieee80211a; // TODO: Could be a or g depending on band
    case Ieee80211PhyType::A:
        return HostapdHwMode::Ieee80211a;
    case Ieee80211PhyType::AC:
        return HostapdHwMode::Ieee80211a;
    case Ieee80211PhyType::AD:
        return HostapdHwMode::Ieee80211ad;
    case Ieee80211PhyType::AX:
        return HostapdHwMode::Ieee80211a;
    case Ieee80211PhyType::BE:
        return HostapdHwMode::Ieee80211a; // TODO: Assuming a, although hostapd docs don't mention it
    default:
        return HostapdHwMode::Unknown;
    }
}

std::string
HostapdHwModeToPropertyValue(HostapdHwMode hwMode) noexcept
{
    switch (hwMode) {
    case HostapdHwMode::Ieee80211b:
        return ProtocolHostapd::PropertyHwModeValueB;
    case HostapdHwMode::Ieee80211g:
        return ProtocolHostapd::PropertyHwModeValueG;
    case HostapdHwMode::Ieee80211a:
        return ProtocolHostapd::PropertyHwModeValueA;
    case HostapdHwMode::Ieee80211ad:
        return ProtocolHostapd::PropertyHwModeValueAD;
    case HostapdHwMode::Ieee80211any:
        return ProtocolHostapd::PropertyHwModeValueAny;
    default: // case HostapdHwMode::Unknown
        LOGE << std::format("Invalid hostapd hw_mode value {}", magic_enum::enum_name(hwMode));
        return "invalid";
    }
}

std::string_view
IeeeFrequencyBandToHostapdBand(Ieee80211FrequencyBand ieeeFrequencyBand) noexcept
{
    switch (ieeeFrequencyBand) {
    case Ieee80211FrequencyBand::TwoPointFourGHz:
        return ProtocolHostapd::PropertySetBandValue2G;
    case Ieee80211FrequencyBand::FiveGHz:
        return ProtocolHostapd::PropertySetBandValue5G;
    case Ieee80211FrequencyBand::SixGHz:
        return ProtocolHostapd::PropertySetBandValue6G;
    default:
        LOGE << std::format("Invalid ieee80211 frequency band value {}", magic_enum::enum_name(ieeeFrequencyBand));
        return "invalid";
    }
}

WpaAuthenticationAlgorithm
Ieee80211AuthenticationAlgorithmToWpaAuthenticationAlgorithm(Ieee80211AuthenticationAlgorithm ieee80211AuthenticationAlgorithm) noexcept
{
    switch (ieee80211AuthenticationAlgorithm) {
    case Ieee80211AuthenticationAlgorithm::OpenSystem:
        return WpaAuthenticationAlgorithm::OpenSystem;
    case Ieee80211AuthenticationAlgorithm::SharedKey:
        return WpaAuthenticationAlgorithm::SharedKey;
    case Ieee80211AuthenticationAlgorithm::FastBssTransition:
        return WpaAuthenticationAlgorithm::Ft;
    case Ieee80211AuthenticationAlgorithm::Sae:
        return WpaAuthenticationAlgorithm::Sae;
    case Ieee80211AuthenticationAlgorithm::Fils:
        return WpaAuthenticationAlgorithm::Fils;
    case Ieee80211AuthenticationAlgorithm::FilsPfs:
        [[fallthrough]];
    case Ieee80211AuthenticationAlgorithm::FilsPublicKey:
        return WpaAuthenticationAlgorithm::FilsSkPfs;
    case Ieee80211AuthenticationAlgorithm::VendorSpecific:
        [[fallthrough]];
    case Ieee80211AuthenticationAlgorithm::Unknown:
        [[fallthrough]];
    default:
        return WpaAuthenticationAlgorithm::Invalid;
    }
}

WpaSecurityProtocol
Ieee80211SecurityProtocolToWpaSecurityProtocol(Ieee80211SecurityProtocol ieee80211SecurityProtocol) noexcept
{
    switch (ieee80211SecurityProtocol) {
    case Ieee80211SecurityProtocol::Wpa:
        return WpaSecurityProtocol::Wpa;
    case Ieee80211SecurityProtocol::Wpa2:
        return WpaSecurityProtocol::Wpa2;
    case Ieee80211SecurityProtocol::Wpa3:
        return WpaSecurityProtocol::Wpa3;
    case Ieee80211SecurityProtocol::Unknown:
        [[fallthrough]];
    default:
        return WpaSecurityProtocol::Unknown;
    }
}

Wpa::WpaKeyManagement
Ieee80211AkmSuiteToWpaKeyManagement(Ieee80211AkmSuite ieee80211AkmSuite) noexcept
{
    switch (ieee80211AkmSuite) {
    case Ieee80211AkmSuite::Ieee8021x:
        return WpaKeyManagement::Ieee80211x;
    case Ieee80211AkmSuite::Psk:
        return WpaKeyManagement::Psk;
    case Ieee80211AkmSuite::Ft8021x:
        return WpaKeyManagement::FtIeee8021x;
    case Ieee80211AkmSuite::FtPsk:
        return WpaKeyManagement::FtPsk;
    case Ieee80211AkmSuite::Ieee8021xSha256:
        return WpaKeyManagement::Ieee8021xSha256;
    case Ieee80211AkmSuite::PskSha256:
        return WpaKeyManagement::PskSha256;
    case Ieee80211AkmSuite::Sae:
        return WpaKeyManagement::Sae;
    case Ieee80211AkmSuite::FtSae:
        return WpaKeyManagement::FtSae;
    case Ieee80211AkmSuite::Ieee8021xSuiteB:
        return WpaKeyManagement::Ieee80211xSuiteB;
    case Ieee80211AkmSuite::Ieee8011xSuiteB192:
        return WpaKeyManagement::Ieee80211xSuiteB192;
    case Ieee80211AkmSuite::Ft8021xSha384:
        return WpaKeyManagement::FtIeee8021xSha384;
    case Ieee80211AkmSuite::FilsSha256:
        return WpaKeyManagement::FilsSha256;
    case Ieee80211AkmSuite::FilsSha384:
        return WpaKeyManagement::FilsSha384;
    case Ieee80211AkmSuite::FtFilsSha256:
        return WpaKeyManagement::FtFilsSha256;
    case Ieee80211AkmSuite::FtFilsSha384:
        return WpaKeyManagement::FilsSha384;
    case Ieee80211AkmSuite::Owe:
        return WpaKeyManagement::Owe;
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
        return WpaKeyManagement::Unknown;
    }
}

WpaCipher
Ieee80211CipherSuiteToWpaCipher(Ieee80211CipherSuite ieee80211CipherSuite) noexcept
{
    switch (ieee80211CipherSuite) {
    case Ieee80211CipherSuite::BipCmac128:
        return WpaCipher::Aes128Cmac;
    case Ieee80211CipherSuite::BipCmac256:
        return WpaCipher::BipCmac256;
    case Ieee80211CipherSuite::BipGmac128:
        return WpaCipher::BipGmac128;
    case Ieee80211CipherSuite::BipGmac256:
        return WpaCipher::BipGmac256;
    case Ieee80211CipherSuite::Ccmp128:
        return WpaCipher::Ccmp;
    case Ieee80211CipherSuite::Ccmp256:
        return WpaCipher::Ccmp256;
    case Ieee80211CipherSuite::Gcmp128:
        return WpaCipher::Gcmp;
    case Ieee80211CipherSuite::Gcmp256:
        return WpaCipher::Gcmp256;
    case Ieee80211CipherSuite::GroupAddressesTrafficNotAllowed:
        return WpaCipher::GtkNotUsed;
    case Ieee80211CipherSuite::Tkip:
        return WpaCipher::Tkip;
    case Ieee80211CipherSuite::UseGroup:
        return WpaCipher::None;
    case Ieee80211CipherSuite::Wep104:
        return WpaCipher::Wep104;
    case Ieee80211CipherSuite::Wep40:
        return WpaCipher::Wep40;
    case Ieee80211CipherSuite::Unknown:
        [[fallthrough]];
    default:
        return WpaCipher::Unknown;
    }
}

std::unordered_map<WpaSecurityProtocol, std::vector<WpaCipher>>
Ieee80211CipherSuitesToWpaCipherSuites(const std::unordered_map<Ieee80211SecurityProtocol, std::vector<Ieee80211CipherSuite>>& ieee80211CipherSuitesConfigurations) noexcept
{
    std::unordered_map<WpaSecurityProtocol, std::vector<WpaCipher>> wpaCipherSuites{};

    for (const auto& [ieee80211SecurityProtocol, ieee80211CipherSuites] : ieee80211CipherSuitesConfigurations) {
        WpaSecurityProtocol wpaSecurityProtocol = Ieee80211SecurityProtocolToWpaSecurityProtocol(ieee80211SecurityProtocol);
        std::vector<WpaCipher> cipherSuitesWpa(std::size(ieee80211CipherSuites));
        std::ranges::transform(ieee80211CipherSuites, std::begin(cipherSuitesWpa), Ieee80211CipherSuiteToWpaCipher);

        wpaCipherSuites.emplace(wpaSecurityProtocol, std::move(cipherSuitesWpa));
    }

    return wpaCipherSuites;
}
} // namespace Microsoft::Net::Wifi
