
#ifndef HOSTAPD_PROTOCOL_HXX
#define HOSTAPD_PROTOCOL_HXX

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <Wpa/ProtocolWpa.hxx>

namespace Wpa
{
/**
 * @brief Describes the state of the interface.
 */
enum class HostapdInterfaceState {
    Uninitialized,
    Disabled,
    Enabled,
    CountryUpdate,
    Acs,
    HtScan,
    Dfs,
    NoIr,
    Unknown
};

enum class HostapdHwMode {
    Unknown,
    Ieee80211b,
    Ieee80211g, // 2.4 GHz
    Ieee80211a, // 5 GHz
    Ieee80211ad,
    Ieee80211any,
};

/**
 * @brief WPA encoding of IEEE 802.11i-2004 protocol types.
 *
 * Note that wpa_supplicant/hostapd configuration don't directly encode WPA3 in this enumeration. Instead, a combination
 * of the 'Wpa2' value below with the 'wpa_key_mgmt' property set to 'SAE' or 'FT-SAE'.
 *
 * Values obtained from hostap/src/common/defs.h.
 */
enum class WpaProtocol : uint32_t {
    Wpa = (1U << 0U),
    Wpa2 = (1U << 1U),
    Wapi = (1U << 2U),
    Osen = (1U << 3U),
    // Aliases
    Rsn = Wpa2,
};

static constexpr auto WpaProtocolMask = 
    std::to_underlying(WpaProtocol::Wpa) |
    std::to_underlying(WpaProtocol::Wpa2) |
    std::to_underlying(WpaProtocol::Wapi) | 
    std::to_underlying(WpaProtocol::Osen);

/**
 * @brief WPA encoding of IEEE 802.11 cipher types.
 *
 * Values obtained from hostap/src/common/defs.h.
 */
enum class WpaCipher : uint32_t {
    None = (1U << 0U),
    Wep40 = (1U << 1U),
    Wep104 = (1U << 2U),
    Tkip = (1U << 3U),
    Ccmp = (1U << 4U),
    Aes128Cmac = (1U << 5U),
    Gcmp = (1U << 6U),
    Sms4 = (1U << 7U),
    Gcmp256 = (1U << 8U),
    Ccmp256 = (1U << 9U),
    // (1U << 10U) intentionally skipped
    BipGmac128 = (1U << 11U),
    BipGmac256 = (1U << 12U),
    BipCmac256 = (1U << 13U),
    GtkNotUsed = (1U << 14U),
};

/**
 * @brief WPA encoding of IEEE 802.11 algorithm types.
 *
 * Values obtained from hostap/src/common/defs.h.
 */
enum class WpaAlgorithm : uint32_t {
    None = 0,
    Wep,
    Tkip,
    Ccmp,
    BipCmac128,
    Gcmp,
    Sms4,
    Krk,
    Gcmp256,
    Ccmp256,
    BipGmac128,
    BipGmac256,
    BipCmac256,
};

/**
 * @brief WPA encoding of IEEE 802.11 key management types.
 *
 * Values obtained from hostap/src/common/defs.h.
 */
enum class WpaKeyManagement : uint32_t {
    Ieee80211x = (1U << 0U),
    Psk = (1U << 1U),
    None = (1U << 2U),
    Ieee80211xNoWpa = (1U << 3U),
    WpaNone = (1U << 4U),
    FtIeee8021x = (1U << 5U),
    FtPsk = (1U << 6U),
    Ieee8021xSha256 = (1U << 7U),
    PskSha256 = (1U << 8U),
    Wps = (1U << 9U),
    Sae = (1U << 10U),
    FtSae = (1U << 11U),
    WapiPsk = (1U << 12U),
    WapiCert = (1U << 13U),
    Cckm = (1U << 14U),
    Osen = (1U << 15U),
    Ieee80211xSuiteB = (1U << 16U),
    Ieee80211xSuiteB192 = (1U << 17U),
    FilsSha256 = (1U << 18U),
    FileSha384 = (1U << 19U),
    FtFilsSha256 = (1U << 20U),
    FtFilsSha384 = (1U << 21U),
    Owe = (1U << 22U),
    Dpp = (1U << 23U),
    FtIeee8021xSha384 = (1U << 24U),
    Pasn = (1U << 25U),
};

static constexpr auto WpaKeyManagementFt =
    std::to_underlying(WpaKeyManagement::Ieee80211x) |
    std::to_underlying(WpaKeyManagement::FtIeee8021x) |
    std::to_underlying(WpaKeyManagement::FtIeee8021xSha384) |
    std::to_underlying(WpaKeyManagement::FtSae) |
    std::to_underlying(WpaKeyManagement::FtFilsSha256) |
    std::to_underlying(WpaKeyManagement::FtFilsSha384);

struct MulticastListenerDiscoveryProtocolInfo
{
    int Id;
    int LinkId;
    std::string MacAddress; // Colon separated hex string with 6 bytes, eg. '%02x:%02x:%02x:%02x:%02x:%02x'.
};

using MldInfo = MulticastListenerDiscoveryProtocolInfo;

struct BssInfo
{
    int Index;
    int NumStations{ 0 };
    std::string Bssid;
    std::string Interface;
    std::string Ssid;

    // Present with:
    //  - CONFIG_IEEE80211BE compilation option
    //  - The AP is part of an AP MLD.
    std::optional<MldInfo> Mld;
};

struct HostapdStatus
{
    // TODO: All types used below are direct translations of the types used in
    // the hostapd code; there may be better representations of them.

    HostapdInterfaceState State{ HostapdInterfaceState::Unknown };

    // Note: The following fields do not yet have parsing code so they are
    // commented out for the time being. Many of them may not be kept if they
    // have no use in the implementation, but for now, this is a complete list
    // of all fields that are returned from the 'STATUS' message.

    // std::string PhyRadio;   // eg. phy=34
    // int Frequency{ 0 };     // eg. 2412
    // int NumberOfStationsNonErp{ 0 };
    // int NumberOfStationsNoShortSlotTime{ 0 };
    // int NumberOfStationsNoShortPreamble{ 0 };
    // int OverlappingLegacyBssCondition{ 0 };
    // int NumberOfStationsHtNoGreenfield{ 0 };
    // int NumberOfStationsNonHt{ 0 };
    // int NumberOfStationsHt20MHz{ 0 };
    // int NumberOfStationsHt40Intolerant{ 0 };
    // int OverlappingLegacyBssConditionInformation{ 0 };
    // uint16_t HtOperationalMode{ 0 };

    // // Present only if a current hwmode is set.
    // std::optional<HostapdHwMode> HwMode;
    // // Present only if first two chars of country code are non-zero.
    // std::optional<std::array<char, 3>> CountryCode;

    // int CacTimeSeconds{ 0 };
    // // Present only if CAC is in progress.
    // std::optional<long> CacTimeRemainingSeconds;

    // uint8_t Channel{ 0 };
    // int EdmgEnable{ 0 };
    // uint8_t EdmgChannel{ 0 };
    // int SecondaryChannel{ 0 };
    int Ieee80211n{ 0 };
    int Ieee80211ac{ 0 };
    int Ieee80211ax{ 0 };
    int Disable11n{ 0 };
    int Disable11ac{ 0 };
    int Disable11ax{ 0 };
    // int Ieee80211be{ 0 };
    // uint16_t BeaconInterval{ 0 };
    // int DtimPeriod{ 0 };

    // // Present with:
    // //  - CONFIG_IEEE80211BE compilation option
    // //  - 'ieee80211be=1' configuration file option
    // std::optional<int> EhtOperationalChannelWidth;
    // std::optional<uint8_t> EhtOperationalCenterFrequencySegment0;

    // // Present with:
    // //  - CONFIG_IEEE80211AX compilation option
    // //  - 'ieee80211ax=1' configuration file option
    // std::optional<int> HeOperationalChannelWidth;
    // std::optional<int> HeOperationalCenterFrequencySegment0;
    // std::optional<int> HeOperationalCenterFrequencySegment1;
    // // Present with:
    // //  - CONFIG_IEEE80211AX compilation option
    // //  - 'ieee80211ax=1' configuration file option
    // //  - 'he_bss_color=1' configuration file option (default: enabled)
    // std::optional<int> HeBssColor;

    // // Present with:
    // //  - 'ieee80211ac=1' configuration file option
    // //  - 'disable_11ac=0' configuration file option
    // std::optional<int> VhtOperationalChannelWidth;
    // std::optional<int> VhtOperationalCenterFrequencySegment0;
    // std::optional<int> VhtOperationalCenterFrequencySegment1;
    // std::optional<uint32_t> VhtCapabilitiesInfo;

    // // Present with:
    // //  - 'ieee80211ac=1' configuration file option
    // //  - 'disable_11ac=0' configuration file option
    // //  - hostapd current operational mode is set to 'ac'
    // std::optional<uint16_t> RxVhtMcsMap;
    // std::optional<uint16_t> TxVhtMcsMap;

    // // Present with:
    // //  - 'ieee80211n=1' configuration file option
    // //  - 'disable_11n=0' configuration file option
    // std::optional<uint16_t> HtCapabilitiesInfo;

    // // Present with:
    // //  - 'ieee80211n=1' configuration file option
    // // - 'disable_11n=0' configuration file option
    // // hostapd current operational mode is set to 'n'
    // std::optional<std::string> HtMcsBitmask;   // Unseparated hex string.

    // // Present with:
    // //  - At least one rate is supported with current configuration.
    // std::optional<std::string> SupportedRates; // Space separated hex string, eg. '%02x %02x ....'.

    // // Present with:
    // //  - The current operational mode supports the currently active frequency.
    // std::optional<unsigned> MaxTxPower;

    // Always present (but may be empty).
    std::vector<BssInfo> Bss;

    // // Present with:
    // //  - Non-zero channel utilization.
    // unsigned ChannelUtilitzationAverage;
};

struct ProtocolHostapd :
    public ProtocolWpa
{
    // Response values for the 'STATE=' property from the "STATUS" command response.
    static constexpr auto ResponsePayloadStatusUninitialized = "UNINITIALIZED";
    static constexpr auto ResponsePayloadStatusDisabled = "DISABLED";
    static constexpr auto ResponsePayloadStatusEnabled = "ENABLED";
    static constexpr auto ResponsePayloadStatusCountryUpdate = "COUNTRY_UPDATE";
    static constexpr auto ResponsePayloadStatusAcs = "ACS";
    static constexpr auto ResponsePayloadStatusHtScan = "HT_SCAN";
    static constexpr auto ResponsePayloadStatusDfs = "DFS";
    static constexpr auto ResponsePayloadStatusNoIr = "NO_IR";
    static constexpr auto ResponsePayloadStatusUnknown = "UNKNOWN";

    // Property names for "GET" commands.
    static constexpr auto PropertyNameVersion = "version";
    static constexpr auto PropertyNameTlsLibrary = "tls_library";
    // Note: this value must be updated if the version of hostapd changes.
    static constexpr auto PropertyVersionValue = "2.10-hostap_2_10";

    // Property names for "SET" commands.
    static constexpr auto PropertyEnabled = "1";
    static constexpr auto PropertyDisabled = "0";

    static constexpr auto PropertyNameSetBand = "setband";
    static constexpr auto PropertySetBandValueAuto = "AUTO";
    static constexpr auto PropertySetBandValue2G = "2G";
    static constexpr auto PropertySetBandValue5G = "5G";
    static constexpr auto PropertySetBandValue6G = "6G";

    static constexpr auto PropertyNameHwMode = "hw_mode";
    static constexpr auto PropertyHwModeValueB = "b";
    static constexpr auto PropertyHwModeValueG = "g";
    static constexpr auto PropertyHwModeValueA = "a";
    static constexpr auto PropertyHwModeValueAD = "ad";
    static constexpr auto PropertyHwModeValueAny = "any";

    static constexpr auto PropertyNameSsid = "ssid";

    static constexpr auto PropertyNameIeee80211N = "ieee80211n";
    static constexpr auto PropertyNameDisable11N = "disable_11n";
    static constexpr auto PropertyNameIeee80211AC = "ieee80211ac";
    static constexpr auto PropertyNameDisable11AC = "disable_11ac";
    static constexpr auto PropertyNameIeee80211AX = "ieee80211ax";
    static constexpr auto PropertyNameDisable11AX = "disable_11ax";
    static constexpr auto PropertyNameWmmEnabled = "wmm_enabled";
    static constexpr auto PropertyNameState = "state";

    // Indexed property names for BSS entries in the "STATUS" response.
    static constexpr auto PropertyNameBss = "bss";
    static constexpr auto PropertyNameBssBssid = "bssid";
    static constexpr auto PropertyNameBssSsid = "ssid";
    static constexpr auto PropertyNameBssNumStations = "num_sta";

    static constexpr auto PropertyNameWpaKeyManagement = "wpa_key_mgmt";
    static constexpr auto PropertyNameWpaPairwise = "wpa_pairwise";
    static constexpr auto PropertyNameWpaPassphrase = "wpa_passphrase";
    static constexpr auto PropertyNameWpaPsk = "wpa_psk";
    static constexpr auto PropertyNameRsnPairwise = "rsn_pairwise";

    // Response properties for the "STATUS" command.
    // Note: all properties must be terminated with the key-value delimeter (=).
    static constexpr auto ResponseStatusPropertyKeyState = PropertyNameState;
    static constexpr auto ResponseStatusPropertyKeyIeee80211N = PropertyNameIeee80211N;
    static constexpr auto ResponseStatusPropertyKeyDisable11N = PropertyNameDisable11N;
    static constexpr auto ResponseStatusPropertyKeyIeee80211AC = PropertyNameIeee80211AC;
    static constexpr auto ResponseStatusPropertyKeyDisableAC = PropertyNameDisable11AC;
    static constexpr auto ResponseStatusPropertyKeyIeee80211AX = PropertyNameIeee80211AX;
    static constexpr auto ResponseStatusPropertyKeyDisableAX = PropertyNameDisable11AX;
};

/**
 * @brief Converts a string to a HostapdInterfaceState.
 *
 * @param state The state string to convert.
 * @return HostapdInterfaceState The corresponding HostapdInterfaceState
 * enumeration value, or HostapdInterfaceState::Unknown if an invalid state was
 * provided.
 */
HostapdInterfaceState
HostapdInterfaceStateFromString(std::string_view state) noexcept;

/**
 * @brief Indicates if the given state describes an operational interface.
 *
 * @param state The state to check.
 * @return true If the interface is operational.
 * @return false If the interface is not operational.
 */
bool
IsHostapdStateOperational(HostapdInterfaceState state) noexcept;
} // namespace Wpa

#endif // HOSTAPD_PROTOCOL_HXX
