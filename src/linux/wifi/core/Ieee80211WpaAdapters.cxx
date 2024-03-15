
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

std::unordered_map<WpaProtocol, WpaCipher>
Ieee80211CipherSuitesToWpaCipherSuites([[maybe_unused]] const std::unordered_map<Ieee80211SecurityProtocol, std::vector<Ieee80211CipherSuite>>& ieee80211CipherSuites) noexcept
{
    std::unordered_map<WpaProtocol, WpaCipher> wpaCipherSuites{};

    // TODO:

    return wpaCipherSuites;
}
} // namespace Microsoft::Net::Wifi
