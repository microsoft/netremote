
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
IeeeProtocolToHostapdHwMode(Ieee80211Protocol ieeeProtocol)
{
    switch (ieeeProtocol) {
    case Ieee80211Protocol::B:
        return HostapdHwMode::Ieee80211b;
    case Ieee80211Protocol::G:
        return HostapdHwMode::Ieee80211g;
    case Ieee80211Protocol::N:
        return HostapdHwMode::Ieee80211a; // TODO: Could be a or g depending on band
    case Ieee80211Protocol::A:
        return HostapdHwMode::Ieee80211a;
    case Ieee80211Protocol::AC:
        return HostapdHwMode::Ieee80211a;
    case Ieee80211Protocol::AD:
        return HostapdHwMode::Ieee80211ad;
    case Ieee80211Protocol::AX:
        return HostapdHwMode::Ieee80211a;
    case Ieee80211Protocol::BE:
        return HostapdHwMode::Ieee80211a; // TODO: Assuming a, although hostapd docs don't mention it
    default:
        return HostapdHwMode::Unknown;
    }
}

std::string
HostapdHwModeToPropertyValue(HostapdHwMode hwMode)
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
IeeeFrequencyBandToHostapdBand(Ieee80211FrequencyBand ieeeFrequencyBand)
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
} // namespace Microsoft::Net::Wifi
