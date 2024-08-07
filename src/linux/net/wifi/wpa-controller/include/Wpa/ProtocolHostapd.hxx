
#ifndef HOSTAPD_PROTOCOL_HXX
#define HOSTAPD_PROTOCOL_HXX

#include <algorithm>
#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
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
 * @brief Initial authentication algorithms supported by hostapd.
 */
enum class WpaAuthenticationAlgorithm : uint32_t {
    Invalid = 0U,
    OpenSystem = 1U << 0U,
    SharedKey = 1U << 1U,
    Leap = 1U << 2U,
    Ft = 1U << 3U,
    Sae = 1U << 4U,
    Fils = 1U << 5U,
    FilsSkPfs = 1U << 6U,
};

/**
 * @brief Array of all authentication algorithm values.
 */
inline constexpr std::array<WpaAuthenticationAlgorithm, 8> WpaAuthenticationAlgorithmsAll = {
    WpaAuthenticationAlgorithm::Invalid,
    WpaAuthenticationAlgorithm::OpenSystem,
    WpaAuthenticationAlgorithm::SharedKey,
    WpaAuthenticationAlgorithm::Leap,
    WpaAuthenticationAlgorithm::Ft,
    WpaAuthenticationAlgorithm::Sae,
    WpaAuthenticationAlgorithm::Fils,
    WpaAuthenticationAlgorithm::FilsSkPfs,
};

/**
 * @brief Array of all unsupported authentication algorithm values.
 */
inline constexpr std::array<WpaAuthenticationAlgorithm, 6> WpaAuthenticationAlgorithmsUnsupported = {
    WpaAuthenticationAlgorithm::Invalid,
    WpaAuthenticationAlgorithm::Leap,
    WpaAuthenticationAlgorithm::Ft,
    WpaAuthenticationAlgorithm::Sae,
    WpaAuthenticationAlgorithm::Fils,
    WpaAuthenticationAlgorithm::FilsSkPfs,
};

/**
 * @brief Numerical bitmask of valid WpaAuthenticationAlgorithm values.
 */
inline constexpr std::underlying_type_t<WpaAuthenticationAlgorithm> WpaAuthenticationAlgorithmMask =
    std::to_underlying(WpaAuthenticationAlgorithm::OpenSystem) |
    std::to_underlying(WpaAuthenticationAlgorithm::SharedKey);

/**
 * @brief Determine if the specified WpaAuthenticationAlgorithm is supported by hostapd.
 *
 * @param WpaSecurityProtocol The WpaAuthenticationAlgorithm to check.
 * @return true The authentication algorithm is supported.
 * @return false The authentication algorithm is not supported.
 */
constexpr bool
IsWpaAuthenticationAlgorithmSupported(WpaAuthenticationAlgorithm wpaAuthenticationAlgorithm) noexcept
{
    return !std::ranges::contains(WpaAuthenticationAlgorithmsUnsupported, wpaAuthenticationAlgorithm);
}

/**
 * @brief WPA encoding of IEEE 802.11i-2004 protocol types.
 *
 * Note that hostapd configuration doesn't directly encode WPA3 in this enumeration. Instead, a combination of the
 * 'Wpa2' value below with the 'wpa_key_mgmt' property set to 'SAE' or 'FT-SAE'.
 *
 * Values obtained from hostap/src/common/defs.h.
 */
enum class WpaSecurityProtocol : uint32_t {
    Unknown = 0,
    Wpa = (1U << 0U),
    Wpa2 = (1U << 1U),
    Wpa3 = (1U << 1U), // intentionally the same as Wpa3, see note above.
};

/**
 * @brief Determine if the specified WpaSecurityProtocol is supported by hostapd.
 *
 * @param wpaSecurityProtocol The WpaSecurityProtocol to check.
 * @return true The protocol is supported.
 * @return false The protocol is not supported.
 */
constexpr bool
IsWpaSecurityProtocolSupported(WpaSecurityProtocol wpaSecurityProtocol) noexcept
{
    return wpaSecurityProtocol != WpaSecurityProtocol::Unknown;
}

/**
 * @brief Numerical bitmask of valid WpaSecurityProtocol values.
 */
static constexpr std::underlying_type_t<WpaSecurityProtocol> WpaSecurityProtocolMask =
    std::to_underlying(WpaSecurityProtocol::Wpa) |
    std::to_underlying(WpaSecurityProtocol::Wpa2) |
    std::to_underlying(WpaSecurityProtocol::Wpa3);

/**
 * @brief WPA encoding of IEEE 802.11 cipher types.
 *
 * Values obtained from hostap/src/common/defs.h.
 */
enum class WpaCipher : uint32_t {
    Unknown = 0U,
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
 * @brief Array of all WpaCipher values.
 *
 * magic_enum::enum_values() cannot be used since the enum values exceed [MAGIC_ENUM_RANGE_MIN, MAGIC_ENUM_RANGE_MAX].
 */
inline constexpr std::array<WpaCipher, 15> WpaCiphersAll = {
    WpaCipher::Unknown,
    WpaCipher::None,
    WpaCipher::Wep40,
    WpaCipher::Wep104,
    WpaCipher::Tkip,
    WpaCipher::Ccmp,
    WpaCipher::Aes128Cmac,
    WpaCipher::Gcmp,
    WpaCipher::Sms4,
    WpaCipher::Gcmp256,
    WpaCipher::Ccmp256,
    WpaCipher::BipGmac128,
    WpaCipher::BipGmac256,
    WpaCipher::BipCmac256,
    WpaCipher::GtkNotUsed,
};

/**
 * @brief Array of all unsupported WpaCipher values.
 */
inline constexpr std::array<WpaCipher, 5> WpaCiphersUnsupported = {
    WpaCipher::Unknown,
    WpaCipher::None,
    WpaCipher::Wep40,
    WpaCipher::Wep104,
    WpaCipher::Sms4,
};

/**
 * @brief Detrmine if the specified WpaCipher is supported by hostapd.
 *
 * @param wpaCipher The WpaCipher to check.
 * @return true The cipher is supported.
 * @return false The cipher is not supported.
 */
constexpr bool
IsWpaCipherSupported(WpaCipher wpaCipher) noexcept
{
    return !std::ranges::contains(WpaCiphersUnsupported, wpaCipher);
}

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
    Unknown = 0U,
    Ieee8021x = (1U << 0U),
    Psk = (1U << 1U),
    None = (1U << 2U),
    Ieee8021xNoWpa = (1U << 3U),
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
    Ieee8021xSuiteB = (1U << 16U),
    Ieee8021xSuiteB192 = (1U << 17U),
    FilsSha256 = (1U << 18U),
    FilsSha384 = (1U << 19U),
    FtFilsSha256 = (1U << 20U),
    FtFilsSha384 = (1U << 21U),
    Owe = (1U << 22U),
    Dpp = (1U << 23U),
    FtIeee8021xSha384 = (1U << 24U),
    Pasn = (1U << 25U),
};

/**
 * @brief Array of all WpaKeyManagement values.
 *
 * magic_enum::enum_values() cannot be used since the enum values exceed [MAGIC_ENUM_RANGE_MIN, MAGIC_ENUM_RANGE_MAX].
 */
inline constexpr std::array<WpaKeyManagement, 27> AllWpaKeyManagements = {
    WpaKeyManagement::Unknown,
    WpaKeyManagement::Ieee8021x,
    WpaKeyManagement::Psk,
    WpaKeyManagement::None,
    WpaKeyManagement::Ieee8021xNoWpa,
    WpaKeyManagement::WpaNone,
    WpaKeyManagement::FtIeee8021x,
    WpaKeyManagement::FtPsk,
    WpaKeyManagement::Ieee8021xSha256,
    WpaKeyManagement::PskSha256,
    WpaKeyManagement::Wps,
    WpaKeyManagement::Sae,
    WpaKeyManagement::FtSae,
    WpaKeyManagement::WapiPsk,
    WpaKeyManagement::WapiCert,
    WpaKeyManagement::Cckm,
    WpaKeyManagement::Osen,
    WpaKeyManagement::Ieee8021xSuiteB,
    WpaKeyManagement::Ieee8021xSuiteB192,
    WpaKeyManagement::FilsSha256,
    WpaKeyManagement::FilsSha384,
    WpaKeyManagement::FtFilsSha256,
    WpaKeyManagement::FtFilsSha384,
    WpaKeyManagement::Owe,
    WpaKeyManagement::Dpp,
    WpaKeyManagement::FtIeee8021xSha384,
    WpaKeyManagement::Pasn,
};

/**
 * @brief Array of all WpaKeyManagement values accepted by hostapd the control socket.
 *
 * magic_enum::enum_values() cannot be used since the enum values exceed [MAGIC_ENUM_RANGE_MIN, MAGIC_ENUM_RANGE_MAX].
 */
inline constexpr std::array<WpaKeyManagement, 20> AllWpaKeyManagementsValid = {
    WpaKeyManagement::Unknown,
    WpaKeyManagement::Ieee8021x,
    WpaKeyManagement::Psk,
    WpaKeyManagement::FtIeee8021x,
    WpaKeyManagement::FtPsk,
    WpaKeyManagement::Ieee8021xSha256,
    WpaKeyManagement::PskSha256,
    WpaKeyManagement::Sae,
    WpaKeyManagement::FtSae,
    WpaKeyManagement::Osen,
    WpaKeyManagement::Ieee8021xSuiteB,
    WpaKeyManagement::Ieee8021xSuiteB192,
    WpaKeyManagement::FilsSha256,
    WpaKeyManagement::FilsSha384,
    WpaKeyManagement::FtFilsSha256,
    WpaKeyManagement::FtFilsSha384,
    WpaKeyManagement::Owe,
    WpaKeyManagement::Dpp,
    WpaKeyManagement::FtIeee8021xSha384,
    WpaKeyManagement::Pasn,
};

/**
 * @brief A bitmask containing all valid WpaKeyManagement values supporting fast-transition (FT).
 */
static constexpr std::underlying_type_t<WpaKeyManagement> WpaKeyManagementMaskFt =
    std::to_underlying(WpaKeyManagement::Ieee8021x) |
    std::to_underlying(WpaKeyManagement::FtIeee8021x) |
    std::to_underlying(WpaKeyManagement::FtPsk) |
    std::to_underlying(WpaKeyManagement::FtIeee8021xSha384) |
    std::to_underlying(WpaKeyManagement::FtSae) |
    std::to_underlying(WpaKeyManagement::FtFilsSha256) |
    std::to_underlying(WpaKeyManagement::FtFilsSha384);

/**
 * @brief All valid WpaKeyManagement values supporting fast-transition (FT).
 */
static constexpr std::initializer_list<WpaKeyManagement> WpaKeyManagementFt = {
    WpaKeyManagement::Ieee8021x,
    WpaKeyManagement::FtIeee8021x,
    WpaKeyManagement::FtPsk,
    WpaKeyManagement::FtIeee8021xSha384,
    WpaKeyManagement::FtSae,
    WpaKeyManagement::FtFilsSha256,
    WpaKeyManagement::FtFilsSha384,
};

/**
 * @brief All valid WpaKeyManagement values supporting IEEE 802.1X.
 */
static constexpr std::initializer_list<WpaKeyManagement> WpaKeyManagementIeee8021x = {
    WpaKeyManagement::Ieee8021x,
    WpaKeyManagement::FtIeee8021x,
    WpaKeyManagement::FtIeee8021xSha384,
    WpaKeyManagement::Cckm,
    WpaKeyManagement::Osen,
    WpaKeyManagement::Ieee8021xSha256,
    WpaKeyManagement::Ieee8021xSuiteB,
    WpaKeyManagement::Ieee8021xSuiteB192,
    WpaKeyManagement::FilsSha256,
    WpaKeyManagement::FilsSha384,
    WpaKeyManagement::FtFilsSha256,
    WpaKeyManagement::FtFilsSha384,
};

/**
 * @brief Determines if the specified key management value is part of the fast-transition (FT) class of authentication
 * and key management schemes.
 *
 * @param wpaKeyManagement The key management value to check
 * @return true The specified key management value supports fast-transition (FT).
 * @return false The specified key management value does not support fast-transition (FT).
 */
static constexpr bool
IsKeyManagementFastTransition(WpaKeyManagement wpaKeyManagement)
{
    return std::ranges::contains(WpaKeyManagementFt, wpaKeyManagement);
}

/**
 * @brief Determines if the specified key management value is part of the IEEE 802.1X class of authentication and key
 * management schemes.
 *
 * @param wpaKeyManagement The key management value to check.
 * @return true The specified key management value supports IEEE 802.1X.
 * @return false The specified key management value does not support IEEE 802.1X.
 */
static constexpr bool
IsKeyManagementIeee8021x(WpaKeyManagement wpaKeyManagement)
{
    return std::ranges::contains(WpaKeyManagementIeee8021x, wpaKeyManagement);
}

/**
 * @brief Management frame protection levels.
 */
enum class ManagementFrameProtection : uint8_t {
    Unknown = 0x80U,
    None = 0x00U,
    Optional = 0x01U,
    Required = 0x02U,
};

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

struct HostapdBssConfiguration
{
    // TODO: All types used below are direct translations of the types used in
    // the hostapd code; there may be better representations of them.

    std::string Bssid;
    std::string Ssid;

    // Note: The following fields do not yet have parsing code so they are
    // commented out for the time being. Many of them may not be kept if they
    // have no use in the implementation, but for now, this is a complete list
    // of all fields that are returned from the 'GET_CONFIG' message.

    // // Present with:
    // //  - CONFIG_WPS compilation option
    // std::optional<int> WpsState;

    // // Present with:
    // //  - Value for current 'wps_state" is set.
    // //  - Value for current 'wpa' is set.
    // std::optional<WpaPreSharedKey> WpaPassphrase;
    // std::optional<WpaPreSharedKey> WpaPsk;

    // // Present with:
    // //  - 'multi_ap=1' OR 'multi_ap=3' configuration file option
    // std::optional<int> MultiApMode;
    // std::optional<std::string> MultiApBackhaulSsid;

    // // Present with:
    // //  - 'multi_ap=1' OR 'multi_ap=3' configuration file option
    // //  - Value for current 'wps_state' is set.
    // //  - Value for current 'wpa' is set.
    // std::optional<WpaPreSharedKey> MultiApBackhaulWpaPassphrase;
    // std::optional<WpaPreSharedKey> MultiApBackhaulWpaPsk;

    WpaSecurityProtocol Wpa;
    std::vector<WpaKeyManagement> WpaKeyMgmt;
    WpaCipher GroupCipher;
    WpaCipher RsnPairwiseCipher;
    WpaCipher WpaPairwiseCipher;

    // // Present only if 'wpa' and a current 'wpa_deny_ptk0_rekey' value is set.
    // std::optional<int> WpaDenyPtk0Rekey;

    // // Present only if driver supports it and if RSN/WPA2 is used with a CCMP/GCMP pairwise cipher.
    // std::optional<int> ExtendedKeyId;
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
    static constexpr auto PropertyNameInvalid = "invalid";

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

    static constexpr auto PropertyNameAuthenticationAlgorithms = "auth_algs";
    static constexpr auto PropertyNameSsid = "ssid";

    static constexpr auto PropertyNameIeee80211N = "ieee80211n";
    static constexpr auto PropertyNameDisable11N = "disable_11n";
    static constexpr auto PropertyNameIeee80211AC = "ieee80211ac";
    static constexpr auto PropertyNameDisable11AC = "disable_11ac";
    static constexpr auto PropertyNameIeee80211AX = "ieee80211ax";
    static constexpr auto PropertyNameDisable11AX = "disable_11ax";
    static constexpr auto PropertyNameWmmEnabled = "wmm_enabled";
    static constexpr auto PropertyNameState = "state";

    static constexpr auto PropertyNameIeee80211W = "ieee80211w";
    static constexpr auto PropertyValueIeee80211WNone = "0";
    static constexpr auto PropertyValueIeee80211WOptional = "1";
    static constexpr auto PropertyValueIeee80211WRequired = "2";

    static constexpr auto PropertyNameBridgeInterface = "bridge";

    // 802.1X/EAP properties.
    static constexpr auto PropertyNameIeee8021X = "ieee8021x";
    static constexpr auto PropertyNameEapServer = "eap_server";

    // RADIUS properties.
    static constexpr auto PropertyNameOwnIpAddr = "own_ip_addr";
    static constexpr auto PropertyNameNasIdentifier = "nas_identifier";
    static constexpr auto PropertyNameRadiusClientAddr = "radius_client_addr";
    static constexpr auto PropertyNameRadiusClientDev = "radius_client_dev";

    static constexpr auto PropertyNameRadiusAuthServerAddr = "auth_server_addr";
    static constexpr auto PropertyNameRadiusAuthServerPort = "auth_server_port";
    static constexpr auto PropertyNameRadiusAuthServerShared = "auth_server_shared_secret";
    static constexpr auto PropertyNameRadiusAuthReqAttr = "radius_auth_req_attr";

    static constexpr auto PropertyNameRadiusAcctServerAddr = "acct_server_addr";
    static constexpr auto PropertyNameRadiusAcctServerPort = "acct_server_port";
    static constexpr auto PropertyNameRadiusAcctServerShared = "acct_server_shared_secret";
    static constexpr auto PropertyNameRadiusAcctReqAttr = "radius_acct_req_attr";

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

    static constexpr auto PropertyNameSaePassword = "sae_password";
    static constexpr auto PropertyValueSaePasswordKeyPeerMac = "mac";
    static constexpr auto PropertyValueSaePasswordKeyPasswordId = "id";
    static constexpr auto PropertyValueSaePasswordKeyVlanId = "vlanid";
    static constexpr auto PropertyValueSaePasswordClearAll = "";
    static constexpr auto PropertyValueSaeKeyValueSeparator = "|";

    // Property names for "GET_CONFIG" command.
    static constexpr auto PropertyNameKeyManagement = "key_mgmt";
    static constexpr auto PropertyNameGroupCipher = "group_cipher";
    static constexpr auto PropertyNameRsnPairwiseCipher = "rsn_pairwise_cipher";
    static constexpr auto PropertyNameWpaPairwiseCipher = "wpa_pairwise_cipher";

    // Response properties for the "STATUS" command.
    // Note: all properties must be terminated with the key-value delimeter (=).
    static constexpr auto ResponseStatusPropertyKeyState = PropertyNameState;
    static constexpr auto ResponseStatusPropertyKeyIeee80211N = PropertyNameIeee80211N;
    static constexpr auto ResponseStatusPropertyKeyDisable11N = PropertyNameDisable11N;
    static constexpr auto ResponseStatusPropertyKeyIeee80211AC = PropertyNameIeee80211AC;
    static constexpr auto ResponseStatusPropertyKeyDisableAC = PropertyNameDisable11AC;
    static constexpr auto ResponseStatusPropertyKeyIeee80211AX = PropertyNameIeee80211AX;
    static constexpr auto ResponseStatusPropertyKeyDisableAX = PropertyNameDisable11AX;

    // Response properties for the "GET_CONFIG" command.
    // Note: all properties must be terminated with the key-value delimeter (=).
    static constexpr auto ResponseGetConfigPropertyKeyBssid = PropertyNameBssBssid;
    static constexpr auto ResponseGetConfigPropertyKeySsid = PropertyNameBssSsid;
    static constexpr auto ResponseGetConfigPropertyKeyWpa = PropertyNameWpaSecurityProtocol;
    static constexpr auto ResponseGetConfigPropertyKeyWpaKeyMgmt = PropertyNameKeyManagement;
    static constexpr auto ResponseGetConfigPropertyKeyGroupCipher = PropertyNameGroupCipher;
    static constexpr auto ResponseGetConfigPropertyKeyRsnPairwiseCipher = PropertyNameRsnPairwiseCipher;
    static constexpr auto ResponseGetConfigPropertyKeyWpaPairwiseCipher = PropertyNameWpaPairwiseCipher;
};

/**
 * @brief Convert a ManagementFrameProtection value to the corresponding property value string expected by hostapd. The
 * return value may be used for the hostapd property 'ieee80211w'.
 *
 * @param managementFrameProtection The ManagementFrameProtection value to convert.
 * @return constexpr std::string_view
 */
constexpr std::string_view
ManagementFrameProtectionToPropertyValue(ManagementFrameProtection managementFrameProtection) noexcept
{
    switch (managementFrameProtection) {
    case ManagementFrameProtection::None:
        return ProtocolHostapd::PropertyValueIeee80211WNone;
    case ManagementFrameProtection::Optional:
        return ProtocolHostapd::PropertyValueIeee80211WOptional;
    case ManagementFrameProtection::Required:
        return ProtocolHostapd::PropertyValueIeee80211WRequired;
    case ManagementFrameProtection::Unknown:
        [[fallthrough]];
    default:
        return "UNKNOWN";
    }
}

/**
 * @brief Convert a HostapdHwMode value to the corresponding property value string expected by hostapd. The
 * return value may be used for the hostapd property 'hw_mode'.
 *
 * @param hwMode The HostapdHwMode value to convert.
 * @return constexpr std::string_view
 */
constexpr std::string_view
HostapdHwModePropertyValue(HostapdHwMode hwMode) noexcept
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
    case HostapdHwMode::Unknown:
        [[fallthrough]];
    default:
        return "UNKNOWN";
    }
}

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

/**
 * @brief WpaKeyManagement sentinel for an invalid value.
 */
constexpr std::string_view WpaKeyManagementInvalidValue = "UNKNOWN";

/**
 * @brief Convert a WpaKeyManagement value to the corresponding property value string expected by hostapd. The return
 * value may be used for the hostapd property 'wpa_key_mgmt'.
 *
 * @param wpaKeyManagement The WpaKeyManagement value to convert.
 * @return constexpr std::string_view The corresponding hostapd property value string.
 */
constexpr std::string_view
WpaKeyManagementPropertyValue(WpaKeyManagement wpaKeyManagement) noexcept
{
    switch (wpaKeyManagement) {
    case WpaKeyManagement::Ieee8021x:
        return "WPA-EAP";
    case WpaKeyManagement::Psk:
        return "WPA-PSK";
    case WpaKeyManagement::FtIeee8021x:
        return "FT-EAP";
    case WpaKeyManagement::FtPsk:
        return "FT-PSK";
    case WpaKeyManagement::Ieee8021xSha256:
        return "WPA-EAP-SHA256";
    case WpaKeyManagement::PskSha256:
        return "WPA-PSK-SHA256";
    case WpaKeyManagement::Sae:
        return "SAE";
    case WpaKeyManagement::FtSae:
        return "FT-SAE";
    case WpaKeyManagement::Osen:
        return "OSEN";
    case WpaKeyManagement::Ieee8021xSuiteB:
        return "WPA-EAP-SUITE-B";
    case WpaKeyManagement::Ieee8021xSuiteB192:
        return "WPA-EAP-SUITE-B-192";
    case WpaKeyManagement::FilsSha256:
        return "FILS-SHA256";
    case WpaKeyManagement::FilsSha384:
        return "FILS-SHA384";
    case WpaKeyManagement::FtFilsSha256:
        return "FT-FILS-SHA256";
    case WpaKeyManagement::FtFilsSha384:
        return "FT-FILS-SHA384";
    case WpaKeyManagement::Owe:
        return "OWE";
    case WpaKeyManagement::Dpp:
        return "DPP";
    case WpaKeyManagement::FtIeee8021xSha384:
        return "FT-EAP-SHA384";
    case WpaKeyManagement::Pasn:
        return "PASN";
    // Below values are not accepted for nor parsed from the configuration file.
    case WpaKeyManagement::None:
        [[fallthrough]];
    case WpaKeyManagement::Ieee8021xNoWpa:
        [[fallthrough]];
    case WpaKeyManagement::WpaNone:
        [[fallthrough]];
    case WpaKeyManagement::Wps:
        [[fallthrough]];
    case WpaKeyManagement::WapiPsk:
        [[fallthrough]];
    case WpaKeyManagement::WapiCert:
        [[fallthrough]];
    case WpaKeyManagement::Cckm:
        [[fallthrough]];
    default:
        return WpaKeyManagementInvalidValue;
    }
}

/**
 * @brief Convert a string to a single WpaKeyManagement value.
 *
 * @param wpaKeyManagementProperty The hostapd property value string to convert.
 * @return WpaKeyManagement The corresponding WpaKeyManagement value.
 */
WpaKeyManagement
WpaKeyManagementFromPropertyValue(std::string_view wpaKeyManagementProperty) noexcept;

/**
 * @brief Convert a hostapd 'wpa_key_mgmt' property value string to the corresponding WpaKeyManagement value.
 * This string may have several whitespace-separated values, such as "WPA-PSK SAE".
 *
 * @param wpaKeyManagementProperty The hostapd property value string to convert.
 * @return std::vector<WpaKeyManagement> The corresponding WpaKeyManagement values.
 */
std::vector<WpaKeyManagement>
WpaKeyManagementsFromPropertyValue(std::string_view wpaKeyManagementProperty) noexcept;

/**
 * @brief WpaCipher sentinel for an invalid value.
 */
constexpr std::string_view WpaCipherInvalidValue = "UNKNOWN";

/**
 * @brief Convert a WpaCipher value to the corresponding property value string expected by hostapd. The returned value
 * may be used for hostapd properties 'wpa_pairwise' and 'rsn_pairwise'.
 *
 * @param wpaCipher The WpaCipher value to convert.
 * @return constexpr std::string_view The corresponding hostapd property value string.
 */
constexpr std::string_view
WpaCipherPropertyValue(WpaCipher wpaCipher) noexcept
{
    switch (wpaCipher) {
    case WpaCipher::None:
        return "NONE";
    case WpaCipher::Wep40:
        return "WEP40";
    case WpaCipher::Wep104:
        return "WEP104";
    case WpaCipher::Tkip:
        return "TKIP";
    case WpaCipher::Ccmp:
        return "CCMP";
    case WpaCipher::Aes128Cmac:
        return "AES-128-CMAC";
    case WpaCipher::Gcmp:
        return "GCMP";
    case WpaCipher::Gcmp256:
        return "GCMP-256";
    case WpaCipher::Ccmp256:
        return "CCMP-256";
    case WpaCipher::BipGmac128:
        return "BIP-GMAC-128";
    case WpaCipher::BipGmac256:
        return "BIP-GMAC-256";
    case WpaCipher::BipCmac256:
        return "BIP-CMAC-256";
    case WpaCipher::GtkNotUsed:
        return "GTK_NOT_USED";
    case WpaCipher::Sms4:
        [[fallthrough]];
    default:
        return WpaCipherInvalidValue;
    }
}

/**
 * @brief Convert a hostapd cipher property string such as 'wpa_pairwise' and 'rsn_pairwise' to the corresponding WpaCipher value.
 *
 * @param wpaCipher The hostapd property value string to convert.
 * @return constexpr WpaCipher The corresponding WpaCipher value.
 */
constexpr WpaCipher
WpaCipherFromPropertyValue(std::string_view wpaCipherProperty) noexcept
{
    if (wpaCipherProperty == "NONE") {
        return WpaCipher::None;
    } else if (wpaCipherProperty == "WEP40") {
        return WpaCipher::Wep40;
    } else if (wpaCipherProperty == "WEP104") {
        return WpaCipher::Wep104;
    } else if (wpaCipherProperty == "TKIP") {
        return WpaCipher::Tkip;
    } else if (wpaCipherProperty == "CCMP") {
        return WpaCipher::Ccmp;
    } else if (wpaCipherProperty == "AES-128-CMAC") {
        return WpaCipher::Aes128Cmac;
    } else if (wpaCipherProperty == "GCMP") {
        return WpaCipher::Gcmp;
    } else if (wpaCipherProperty == "GCMP-256") {
        return WpaCipher::Gcmp256;
    } else if (wpaCipherProperty == "CCMP-256") {
        return WpaCipher::Ccmp256;
    } else if (wpaCipherProperty == "BIP-GMAC-128") {
        return WpaCipher::BipGmac128;
    } else if (wpaCipherProperty == "BIP-GMAC-256") {
        return WpaCipher::BipGmac256;
    } else if (wpaCipherProperty == "BIP-CMAC-256") {
        return WpaCipher::BipCmac256;
    } else if (wpaCipherProperty == "GTK_NOT_USED") {
        return WpaCipher::GtkNotUsed;
    } else {
        return WpaCipher::Unknown;
    }
}

/**
 * @brief Get the hostapd property name to use to set the cipher for the specified WPA protocol.
 *
 * Hostapd uses different configuration properties for WPA and WPA2/RSN protocols. This function maps the protocol to
 * the associated property name.
 *
 * @param WpaSecurityProtocol The wpa protocol to get the cipher name property for.
 * @return constexpr std::string_view
 */
constexpr std::string_view
WpaCipherPropertyName(WpaSecurityProtocol WpaSecurityProtocol) noexcept
{
    switch (WpaSecurityProtocol) {
    case WpaSecurityProtocol::Wpa:
        return ProtocolHostapd::PropertyNameWpaPairwise;
    case WpaSecurityProtocol::Wpa2:
        // case WpaSecurityProtocol::Wpa3: // duplicate case value not allowed
        return ProtocolHostapd::PropertyNameRsnPairwise;
    default:
        return ProtocolHostapd::PropertyNameInvalid;
    }
}

/**
 * @brief Get the hostapd property value for the specified WpaSecurityProtocol.
 *
 * @param WpaSecurityProtocol The WpaSecurityProtocol to get the property value for.
 * @return constexpr std::underlying_type_t<WpaSecurityProtocol>
 */
constexpr std::underlying_type_t<WpaSecurityProtocol>
WpaSecurityProtocolPropertyValue(WpaSecurityProtocol WpaSecurityProtocol) noexcept
{
    return std::to_underlying(WpaSecurityProtocol);
}

/**
 * @brief Get the hostapd property value for the specified WpaAuthenticationAlgorithm.
 *
 * @param wpaAuthenticationAlgorithm The WpaAuthenticationAlgorithm to get the property value for.
 * @return constexpr std::underlying_type_t<WpaAuthenticationAlgorithm>
 */
constexpr std::underlying_type_t<WpaAuthenticationAlgorithm>
WpaAuthenticationAlgorithmPropertyValue(WpaAuthenticationAlgorithm wpaAuthenticationAlgorithm) noexcept
{
    return std::to_underlying(wpaAuthenticationAlgorithm);
}

static constexpr std::size_t WpaPskValueLength = 64;
static constexpr std::size_t WpaPskPassphraseLengthMin = 8;
static constexpr std::size_t WpaPskPassphraseLengthMax = 63;

using WpaPskPassphraseT = std::string;
using WpaPskValueT = std::array<char, WpaPskValueLength>;

/**
 * @brief Pre-shared key (PSK).
 */
using WpaPreSharedKey = std::variant<WpaPskPassphraseT, WpaPskValueT>;

/**
 * @brief Get the hostapd property value for the specified WpaPreSharedKey. The returned value
 * may be used for the hostapd property 'wpa_psk' or 'wpa_passphrase'.
 *
 * @param wpaPreSharedKey The WpaPreSharedKey to get the property value for.
 * @return std::string
 */
std::string
WpaPreSharedKeyPropertyValue(const WpaPreSharedKey& wpaPreSharedKey);

/**
 * @brief Get the hostapd property and corresponding value for the specified WpaPreSharedKey.
 *
 * @param wpaPreSharedKey The WpaPreSharedKey to get the property key and value for.
 * @return std::pair<std::string_view, std::string>
 */
std::pair<std::string_view, std::string>
WpaPreSharedKeyPropertyKeyAndValue(const WpaPreSharedKey& wpaPreSharedKey);

/**
 * @brief SAE password entry.
 */
struct SaePassword
{
    std::string Credential;
    std::optional<std::string> PasswordId;
    std::optional<std::string> PeerMacAddress;
    std::optional<int32_t> VlanId;
};

/**
 * @brief Type of endpoint used for RADIUS.
 */
enum class RadiusEndpointType {
    Unknown,
    Authentication,
    Accounting,
};

/**
 * @brief Default port for RADIUS authentication servers.
 */
static constexpr uint16_t RadiusAuthenticationPortDefault = 1812;

/**
 * @brief Default port for RADIUS accounting servers.
 */
static constexpr uint16_t RadiusAccountingPortDefault = 1813;

/**
 * @brief Configuration values for a RADIUS endpoint, either a request or accounting server.
 */
struct RadiusEndpointConfiguration
{
    RadiusEndpointType Type{ RadiusEndpointType::Unknown };
    std::string Address{};
    std::optional<uint16_t> Port{};
    std::string SharedSecret{};
};

/**
 * @brief Get the radius endpoint property names given a radius endpoint type.
 *
 * @param type The type of radius endpoint.
 * @return constexpr std::tuple<std::string_view, std::string_view, std::string_view>
 */
constexpr std::tuple<std::string_view, std::string_view, std::string_view>
GetRadiusEndpointPropertyNames(RadiusEndpointType type)
{
    switch (type) {
    case RadiusEndpointType::Authentication:
        return { ProtocolHostapd::PropertyNameRadiusAuthServerAddr,
            ProtocolHostapd::PropertyNameRadiusAuthServerPort,
            ProtocolHostapd::PropertyNameRadiusAuthServerShared };
    case RadiusEndpointType::Accounting:
        return { ProtocolHostapd::PropertyNameRadiusAcctServerAddr,
            ProtocolHostapd::PropertyNameRadiusAcctServerPort,
            ProtocolHostapd::PropertyNameRadiusAcctServerShared };
    default:
        return { ProtocolHostapd::PropertyNameInvalid,
            ProtocolHostapd::PropertyNameInvalid,
            ProtocolHostapd::PropertyNameInvalid };
    }
}
} // namespace Wpa

#endif // HOSTAPD_PROTOCOL_HXX
