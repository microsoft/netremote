
#ifndef PROTOCOL_WPA_HXX
#define PROTOCOL_WPA_HXX

#include <cstdint>
#include <string_view>
#include <utility>

namespace Wpa
{
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

/**
 * @brief Shared/common WPA daemon protocol data and helpers.
 */
struct ProtocolWpa
{
    // The delimeter used to separate key-value pairs in the WPA control protocol.
    static constexpr auto KeyValueDelimiter = '=';
    static constexpr auto KeyValueLineDelimeter = '\n';
    static constexpr auto KeyValueIndexDelimeterStart = '[';
    static constexpr auto KeyValueIndexDelimeterEnd = ']';

    // Command payloads.
    static constexpr auto CommandPayloadPing = "PING";
    static constexpr auto CommandPayloadEnable = "ENABLE";
    static constexpr auto CommandPayloadDisable = "DISABLE";
    static constexpr auto CommandPayloadStatus = "STATUS";
    static constexpr auto CommandPayloadTerminate = "TERMINATE";
    static constexpr auto CommandPayloadRelog = "RELOG";
    static constexpr auto CommandPayloadGet = "GET";
    static constexpr auto CommandPayloadSet = "SET";
    static constexpr auto CommandPayloadNote = "NOTE";
    static constexpr auto CommandPayloadMib = "MIB";
    static constexpr auto CommandPayloadPmksa = "PMKSA";
    static constexpr auto CommandPayloadPmksaFlush = "PMKSA_FLUSH";
    static constexpr auto CommandPayloadGetCapability = "GET_CAPABILITY";
    static constexpr auto CommandPayloadUpdateBeacon = "UPDATE_BEACON";
    static constexpr auto CommandPayloadChannelSwitch = "CHAN_SWITCH";
    static constexpr auto CommandPayloadVendor = "VENDOR";
    static constexpr auto CommandPayloadFlush = "FLUSH";
    static constexpr auto CommandPayloadInterfaces = "INTERFACES";
    static constexpr auto CommandPayloadDuplicateNetwork = "DUP_NETWORK";
    static constexpr auto CommandPayloadDriverFlags = "DRIVER_FLAGS";
    static constexpr auto CommandPayloadDriverFlags2 = "DRIVER_FLAGS2";
    static constexpr auto CommandPayloadDeauthenticate = "DEAUTHENTICATE";
    static constexpr auto CommandPayloadDisassociate = "DISASSOCIATE";
    static constexpr auto CommandPayloadAcceptAcl = "ACCEPT_ACL";
    static constexpr auto CommandPayloadDenyAcl = "DENY_ACL";
    static constexpr auto CommandPayloadAclAddMac = "ADD_MAC";
    static constexpr auto CommandPayloadAclDeleteMac = "DEL_MAC";
    static constexpr auto CommandPayloadAclShow = "SHOW";
    static constexpr auto CommandPayloadAclClear = "CLEAR";
    static constexpr auto CommandPayloadLogLevel = "LOG_LEVEL";
    static constexpr auto CommandPayloadReload = "RELOAD";

    // Common property names.
    static constexpr auto PropertyNameWpaProtocol = "wpa";

    // Response payloads.
    static constexpr auto ResponsePayloadOk = "OK";
    static constexpr auto ResponsePayloadFail = "FAIL";
    static constexpr auto ResponsePayloadPing = "PONG";

    /**
     * @brief Determines if a response payload indicates success.
     *
     * @param response The response payload to check.
     * @return true
     * @return false
     */
    static bool
    IsResponseOk(std::string_view response);

    /**
     * @brief Determines if a response payload indicates failure.
     *
     * @param response The response payload to check.
     * @return true
     * @return false
     */
    static bool
    IsResponseFail(std::string_view response);
};
} // namespace Wpa

#endif // PROTOCOL_WPA_HXX
