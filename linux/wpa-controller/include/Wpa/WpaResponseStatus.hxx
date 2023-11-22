
#ifndef WPA_RESPONSE_STATUS_HXX
#define WPA_RESPONSE_STATUS_HXX

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <Wpa/WpaResponse.hxx>
#include <Wpa/ProtocolHostapd.hxx>

namespace Wpa
{
struct WpaResponseStatus
    : public WpaResponse
{
    WpaResponseStatus() = default;

    // TODO: All types used below are direct translations of the types used in
    // the hostapd code; there may be better representations of them.

// Always-present fields.
    HostapdInterfaceState State{ HostapdInterfaceState::Unknown };
    std::string PhyRadio;   // eg. phy=34
    int Frequency{ 0 };     // eg. 2412
    int NumberOfStationsNonErp{ 0 };
    int NumberOfStationsNoShortSlotTime{ 0 };
    int NumberOfStationsNoShortPreamble{ 0 };
    int OverlappingLegacyBssCondition{ 0 };
    int NumberOfStationsHtNoGreenfield{ 0 };
    int NumberOfStationsNonHt{ 0 };
    int NumberOfStationsHt20MHz{ 0 };
    int NumberOfStationsHt40Intolerant{ 0 };
    int OverlappingLegacyBssConditionInformation{ 0 };
    uint16_t HtOperationalMode{ 0 };

    // Present only if a current hwmode is set.
    std::optional<HostapdHwMode> HwMode;
    // Present only if first rwo chars of country code are non-zero.
    std::optional<std::array<char, 3>> CountryCode;

    int CacTimeSeconds{ 0 };
    // Present only if CAC is in progress.
    std::optional<long> CacTimeRemainingSeconds; 

    uint8_t Channel{ 0 };
    int EdmgEnable{ 0 };
    uint8_t EdmgChannel{ 0 };
    int SecondaryChannel{ 0 };
    int Ieee80211n{ 0 };
    int Ieee80211ac{ 0 };
    int Ieee80211ax{ 0 };
    int Ieee80211be{ 0 };
    uint16_t BeaconInterval{ 0 };
    int DtimPeriod{ 0 };

    // Present with:
    //  - CONFIG_IEEE80211BE compilation option
    //  - 'ieee80211be=1' configuration file option
    std::optional<int> EhtOperationalChannelWidth;
    std::optional<uint8_t> EhtOperationalCenterFrequencySegment0;

    // Present with:
    //  - CONFIG_IEEE80211AX compilation option
    //  - 'ieee80211ax=1' configuration file option
    std::optional<int> HeOperationalChannelWidth;
    std::optional<int> HeOperationalCenterFrequencySegment0;
    std::optional<int> HeOperationalCenterFrequencySegment1;
    // Present with:
    //  - CONFIG_IEEE80211AX compilation option
    //  - 'ieee80211ax=1' configuration file option
    //  - 'he_bss_color=1' configuration file option (default: enabled)
    std::optional<int> HeBssColor;

    // Present with:
    //  - 'ieee80211ac=1' configuration file option
    //  - 'disable_11ac=0' configuration file option
    std::optional<int> VhtOperationalChannelWidth;
    std::optional<int> VhtOperationalCenterFrequencySegment0;
    std::optional<int> VhtOperationalCenterFrequencySegment1;
    std::optional<uint32_t> VhtCapabilitiesInfo;

    // Present with:
    //  - 'ieee80211ac=1' configuration file option
    //  - 'disable_11ac=0' configuration file option
    //  - hostapd current operational mode is set to 'ac'
    std::optional<uint16_t RxVhtMcsMap;
    std::optional<uint16_t TxVhtMcsMap;

    // Present with:
    //  - 'ieee80211n=1' configuration file option
    //  - 'disable_11n=0' configuration file option
    std::optional<uint16_t HtCapabilitiesInfo;

    // Present with:
    //  - 'ieee80211n=1' configuration file option
    // - 'disable_11n=0' configuration file option
    // hostapd current operational mode is set to 'n'
    std::optional<std::string HtMcsBitmask;   // Unseparated hex string.

    // Present with:
    //  - At least one rate is supported with current configuration.
    std::optional<std::string SupportedRates; // Space separated hex string, eg. '%02x %02x ....'.


    // Present with:
    //  - The current operational mode supports the currently active frequency.
    std::optional<unsigned MaxTxPower;

    // Always present (but may be empty).
    std::vector<BssInfo> Bss;

    // Present with:
    //  - Non-zero channel utilization.
    unsigned ChannelUtilitzationAverage;
};
} // namespace Wpa

#endif // WPA_RESPONSE_COMMAND_STATUS_HXX
