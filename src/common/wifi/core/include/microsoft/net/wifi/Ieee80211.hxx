
#ifndef IEEE_80211_HXX
#define IEEE_80211_HXX

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <optional>
#include <string>
#include <utility>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Overall security protocol for an IEEE 802.11 network.
 */
enum class Ieee80211SecurityProtocol {
    Unknown,
    Wpa,
    Wpa2,
    Wpa3,
};

enum class Ieee80211FrequencyBand {
    Unknown,
    TwoPointFourGHz,
    FiveGHz,
    SixGHz,
};

namespace Literals
{
#ifndef _MSC_VER
/**
 * @brief User-defined literal operator for allowing use of _GHz to specify
 * frequency band enumeration values.
 */
inline constexpr Microsoft::Net::Wifi::Ieee80211FrequencyBand
operator"" _GHz(long double value) noexcept
{
    /**
     * @brief Floating point values with non-zero fractional parts cannot be
     * compared for equality directly due to binary floating point encoding,
     * which approximates decimal values.
     *
     * As such, a tolerance value is used to determine if the specified
     * value is practically equal to the expected value, given the tolerance.
     * The below tolerance value(s) were defined empirically using a few
     * different processors. Since this function only needs to map to an
     * enumeration value, the range of the tolerance value need not be very
     * precise.
     */
    constexpr auto TwoPointFourGHzTolerance = 8.89045781438113635886e-17L;

    if (std::fabs(value - 2.4) <= TwoPointFourGHzTolerance) {
        return Microsoft::Net::Wifi::Ieee80211FrequencyBand::TwoPointFourGHz;
    } else if (value == 5.0) {
        return Microsoft::Net::Wifi::Ieee80211FrequencyBand::FiveGHz;
    } else if (value == 6.0) {
        return Microsoft::Net::Wifi::Ieee80211FrequencyBand::SixGHz;
    } else {
        return Microsoft::Net::Wifi::Ieee80211FrequencyBand::Unknown;
    }
}

#endif // _MSC_VER

/**
 * @brief User-defined literal operator for allowing use of _MHz to specify
 * frequency band enumeration values.
 */
inline constexpr Microsoft::Net::Wifi::Ieee80211FrequencyBand
operator"" _MHz(unsigned long long int value) noexcept
{
    if (value == 2400) {
        return Microsoft::Net::Wifi::Ieee80211FrequencyBand::TwoPointFourGHz;
    } else if (value == 5000) {
        return Microsoft::Net::Wifi::Ieee80211FrequencyBand::FiveGHz;
    } else if (value == 6000) {
        return Microsoft::Net::Wifi::Ieee80211FrequencyBand::SixGHz;
    } else {
        return Microsoft::Net::Wifi::Ieee80211FrequencyBand::Unknown;
    }
}
} // namespace Literals

enum class Ieee80211PhyType {
    Unknown,
    B,
    G,
    N,
    A,
    AC,
    AD,
    AX,
    BE,
};

/**
 * @brief OUI for IEEE 802.11 organization.
 */
static constexpr uint32_t OuiIeee80211 = 0x00000FAC;

/**
 * @brief OUI placeholder for signaling invalid values.
 */
static constexpr uint32_t OuiInvalid = 0x00FFFFFF;

/**
 * @brief Helper function to construct an IEEE cipher or AKM suite value.
 *
 * The OUI is 3 octets and the suite ID is 1 octet.
 *
 * @param oui The OUI value.
 * @param suiteId The suite ID value.
 * @return constexpr uint32_t
 */
constexpr uint32_t
MakeIeeeSuite(uint32_t oui, uint8_t suiteId)
{
    constexpr uint32_t OuiShiftAmount = 8;
    constexpr uint32_t OuiMask = 0x00FFFFFF;

    return ((oui & OuiMask) << OuiShiftAmount) | suiteId;
}

/**
 * @brief Helper function to construct an IEEE 802.11 cipher suite value. This uses the IEEE 802.11 OUI.
 *
 * @param suiteId The suite ID value.
 * @return constexpr uint32_t
 */
constexpr uint32_t
MakeIeee80211Suite(uint8_t suiteId)
{
    return MakeIeeeSuite(OuiIeee80211, suiteId);
}

/**
 * @brief Authentication algorithms. Each entry denotes a single algorithm, not a suite.
 *
 * Defined in IEEE 802.11-2020, Section 9.4.1.1, Figure 9-82.
 */
enum class Ieee80211AuthenticationAlgorithm : uint16_t {
    Unknown = 0x0000,
    OpenSystem = 0x0001,
    SharedKey = 0x0002,
    FastBssTransition = 0x0003,
    Sae = 0x0004,
    Fils = 0x0005,
    FilsPfs = 0x0006,
    FilsPublicKey = 0x0007,
    VendorSpecific = 0xFFFF,
};

/**
 * @brief AKM suite identifiers or "selectors".
 *
 * Defined in IEEE 802.11-2020, Section 9.4.2.24.3, Table 9-151.
 */
static constexpr uint8_t Ieee80211AkmSuiteIdReserved0 = 0;
static constexpr uint8_t Ieee80211AkmSuiteId8021x = 1;
static constexpr uint8_t Ieee80211AkmSuiteIdPsk = 2;
static constexpr uint8_t Ieee80211AkmSuiteIdFt8021x = 3;
static constexpr uint8_t Ieee80211AkmSuiteIdFtPsk = 4;
static constexpr uint8_t Ieee80211AkmSuiteId8021xSha256 = 5;
static constexpr uint8_t Ieee80211AkmSuiteIdPskSha256 = 6;
static constexpr uint8_t Ieee80211AkmSuiteIdTdls = 7;
static constexpr uint8_t Ieee80211AkmSuiteIdSae = 8;
static constexpr uint8_t Ieee80211AkmSuiteIdFtSae = 9;
static constexpr uint8_t Ieee80211AkmSuiteIdApPeerKey = 10;
static constexpr uint8_t Ieee80211AkmSuiteId8021xSuiteB = 11;
static constexpr uint8_t Ieee80211AkmSuiteId8021xSuiteB192 = 12;
static constexpr uint8_t Ieee80211AkmSuiteIdFt8021xSha384 = 13;
static constexpr uint8_t Ieee80211AkmSuiteIdFilsSha256 = 14;
static constexpr uint8_t Ieee80211AkmSuiteIdFilsSha384 = 15;
static constexpr uint8_t Ieee80211AkmSuiteIdFtFilsSha256 = 16;
static constexpr uint8_t Ieee80211AkmSuiteIdFtFilsSha384 = 17;
static constexpr uint8_t Ieee80211AkmSuiteIdOwe = 18;
static constexpr uint8_t Ieee80211AkmSuiteIdFtPskSha384 = 19;
static constexpr uint8_t Ieee80211AkmSuiteIdPskSha384 = 20;
static constexpr uint8_t Ieee80211AkmSuiteIdPasn = 21;

/**
 * @brief IEEE 802.11 Authentication and Key Management (AKM) Suites.
 *
 * Defined in IEEE 802.11-2020, Section 9.4.2.24.3, Table 9-151.
 */
enum class Ieee80211AkmSuite : uint32_t {
    Unknown = MakeIeeeSuite(OuiInvalid, 0),
    Reserved0 = MakeIeee80211Suite(Ieee80211AkmSuiteIdReserved0),
    Ieee8021x = MakeIeee80211Suite(Ieee80211AkmSuiteId8021x),
    Psk = MakeIeee80211Suite(Ieee80211AkmSuiteIdPsk),
    Ft8021x = MakeIeee80211Suite(Ieee80211AkmSuiteIdFt8021x),
    FtPsk = MakeIeee80211Suite(Ieee80211AkmSuiteIdFtPsk),
    Ieee8021xSha256 = MakeIeee80211Suite(Ieee80211AkmSuiteId8021xSha256),
    PskSha256 = MakeIeee80211Suite(Ieee80211AkmSuiteIdPskSha256),
    Tdls = MakeIeee80211Suite(Ieee80211AkmSuiteIdTdls),
    Sae = MakeIeee80211Suite(Ieee80211AkmSuiteIdSae),
    FtSae = MakeIeee80211Suite(Ieee80211AkmSuiteIdFtSae),
    ApPeerKey = MakeIeee80211Suite(Ieee80211AkmSuiteIdApPeerKey),
    Ieee8021xSuiteB = MakeIeee80211Suite(Ieee80211AkmSuiteId8021xSuiteB),
    Ieee8011xSuiteB192 = MakeIeee80211Suite(Ieee80211AkmSuiteId8021xSuiteB192),
    Ft8021xSha384 = MakeIeee80211Suite(Ieee80211AkmSuiteIdFt8021xSha384),
    FilsSha256 = MakeIeee80211Suite(Ieee80211AkmSuiteIdFilsSha256),
    FilsSha384 = MakeIeee80211Suite(Ieee80211AkmSuiteIdFilsSha384),
    FtFilsSha256 = MakeIeee80211Suite(Ieee80211AkmSuiteIdFtFilsSha256),
    FtFilsSha384 = MakeIeee80211Suite(Ieee80211AkmSuiteIdFtFilsSha384),
    Owe = MakeIeee80211Suite(Ieee80211AkmSuiteIdOwe),
    FtPskSha384 = MakeIeee80211Suite(Ieee80211AkmSuiteIdFtPskSha384),
    PskSha384 = MakeIeee80211Suite(Ieee80211AkmSuiteIdPskSha384),
    Pasn = MakeIeee80211Suite(Ieee80211AkmSuiteIdPasn),
};

/**
 * @brief A listing of all known AKMs. Normally, these would be enumerated with magic_enum::enum_values(), however, that
 * only supports enums with values up to UINT16_MAX-1, and the AKM suite underlying type is uint32_t, so cannot be used.
 */
constexpr std::initializer_list<uint32_t> AllIeee80211Akms{
    std::to_underlying(Ieee80211AkmSuite::Unknown),
    std::to_underlying(Ieee80211AkmSuite::Reserved0),
    std::to_underlying(Ieee80211AkmSuite::Ieee8021x),
    std::to_underlying(Ieee80211AkmSuite::Psk),
    std::to_underlying(Ieee80211AkmSuite::Ft8021x),
    std::to_underlying(Ieee80211AkmSuite::FtPsk),
    std::to_underlying(Ieee80211AkmSuite::Ieee8021xSha256),
    std::to_underlying(Ieee80211AkmSuite::PskSha256),
    std::to_underlying(Ieee80211AkmSuite::Tdls),
    std::to_underlying(Ieee80211AkmSuite::Sae),
    std::to_underlying(Ieee80211AkmSuite::FtSae),
    std::to_underlying(Ieee80211AkmSuite::ApPeerKey),
    std::to_underlying(Ieee80211AkmSuite::Ieee8021xSuiteB),
    std::to_underlying(Ieee80211AkmSuite::Ieee8011xSuiteB192),
    std::to_underlying(Ieee80211AkmSuite::Ft8021xSha384),
    std::to_underlying(Ieee80211AkmSuite::FilsSha256),
    std::to_underlying(Ieee80211AkmSuite::FilsSha384),
    std::to_underlying(Ieee80211AkmSuite::FtFilsSha256),
    std::to_underlying(Ieee80211AkmSuite::FtFilsSha384),
    std::to_underlying(Ieee80211AkmSuite::Owe),
    std::to_underlying(Ieee80211AkmSuite::FtPskSha384),
    std::to_underlying(Ieee80211AkmSuite::PskSha384),
    std::to_underlying(Ieee80211AkmSuite::Pasn),
};

/**
 * @brief List of AKM suites that are supported by WPA v1.
 */
constexpr std::initializer_list<Ieee80211AkmSuite> Wpa1AkmSuites{
    Ieee80211AkmSuite::Ieee8021x,
    Ieee80211AkmSuite::Psk,
};

/**
 * @brief List of AKM suites that are supported by WPA v2.
 */
constexpr std::initializer_list<Ieee80211AkmSuite> Wpa2AkmSuites{
    Ieee80211AkmSuite::Ieee8021x,
    Ieee80211AkmSuite::Psk,
};

/**
 * @brief List of AKM suites that are supported by WPA v3.
 */
constexpr std::initializer_list<Ieee80211AkmSuite> Wpa3AkmSuites{
    Ieee80211AkmSuite::Ieee8021x,
    Ieee80211AkmSuite::Ft8021x,
    Ieee80211AkmSuite::FtSae,
    Ieee80211AkmSuite::Ieee8021xSuiteB,
    Ieee80211AkmSuite::Ieee8011xSuiteB192,
    Ieee80211AkmSuite::Sae,
    Ieee80211AkmSuite::Owe,
};

/**
 * @brief Obtain a list of supported AKM suites for a given security protocol.
 *
 * @param securityProtocol The security protocol to obtain AKM suites for.
 * @return constexpr std::initializer_list<Ieee80211AkmSuite>
 */
constexpr std::initializer_list<Ieee80211AkmSuite>
WpaAkmSuites(const Ieee80211SecurityProtocol& securityProtocol)
{
    switch (securityProtocol) {
    case Ieee80211SecurityProtocol::Wpa:
        return Wpa1AkmSuites;
    case Ieee80211SecurityProtocol::Wpa2:
        return Wpa2AkmSuites;
    case Ieee80211SecurityProtocol::Wpa3:
        return Wpa3AkmSuites;
    default:
        return {};
    }
}

/**
 * @brief Cipher suite identifiers or "selectors".
 *
 * Defined in IEEE 802.11-2020, Section 9.4.2.24.2, Table 9-149.
 */
static constexpr uint8_t Ieee80211CipherSuiteIdUseGroup = 0;
static constexpr uint8_t Ieee80211CipherSuiteIdWep40 = 1;
static constexpr uint8_t Ieee80211CipherSuiteIdTkip = 2;
static constexpr uint8_t Ieee80211CipherSuiteIdReserved = 3;
static constexpr uint8_t Ieee80211CipherSuiteCcmp128 = 4;
static constexpr uint8_t Ieee80211CipherSuiteIdWep104 = 5;
static constexpr uint8_t Ieee80211CipherSuiteIdBipCmac128 = 6;
static constexpr uint8_t Ieee80211CipherSuiteIdGroupAddressTrafficNotAllowed = 7;
static constexpr uint8_t Ieee80211CipherSuiteIdGcmp128 = 8;
static constexpr uint8_t Ieee80211CipherSuiteIdGcmp256 = 9;
static constexpr uint8_t Ieee80211CipherSuiteIdCcmp256 = 10;
static constexpr uint8_t Ieee80211CipherSuiteIdBipGmac128 = 11;
static constexpr uint8_t Ieee80211CipherSuiteIdBipGmac256 = 12;
static constexpr uint8_t Ieee80211CipherSuiteIdBipCmac256 = 13;

/**
 * @brief IEEE 802.11 Cipher Suites.
 *
 * Defined in IEEE 802.11-2020, Section 9.4.2.24.2, Table 9-149.
 */
enum class Ieee80211CipherSuite : uint32_t {
    Unknown = MakeIeeeSuite(OuiInvalid, 0),
    BipCmac128 = MakeIeee80211Suite(Ieee80211CipherSuiteIdBipCmac128),
    BipCmac256 = MakeIeee80211Suite(Ieee80211CipherSuiteIdBipCmac256),
    BipGmac128 = MakeIeee80211Suite(Ieee80211CipherSuiteIdBipGmac128),
    BipGmac256 = MakeIeee80211Suite(Ieee80211CipherSuiteIdBipGmac256),
    Ccmp128 = MakeIeee80211Suite(Ieee80211CipherSuiteCcmp128),
    Ccmp256 = MakeIeee80211Suite(Ieee80211CipherSuiteIdCcmp256),
    Gcmp128 = MakeIeee80211Suite(Ieee80211CipherSuiteIdGcmp128),
    Gcmp256 = MakeIeee80211Suite(Ieee80211CipherSuiteIdGcmp256),
    GroupAddressesTrafficNotAllowed = MakeIeee80211Suite(Ieee80211CipherSuiteIdGroupAddressTrafficNotAllowed),
    Tkip = MakeIeee80211Suite(Ieee80211CipherSuiteIdTkip),
    UseGroup = MakeIeee80211Suite(Ieee80211CipherSuiteIdUseGroup),
    Wep104 = MakeIeee80211Suite(Ieee80211CipherSuiteIdWep104),
    Wep40 = MakeIeee80211Suite(Ieee80211CipherSuiteIdWep40),
};

/**
 * @brief List of cipher suites supported by WPA v1.
 */
constexpr std::initializer_list<Ieee80211CipherSuite> Wpa1CipherSuites{
    Ieee80211CipherSuite::Tkip,
};

/**
 * @brief List of cipher suites supported by WPA v2.
 */
constexpr std::initializer_list<Ieee80211CipherSuite> Wpa2CipherSuites{
    Ieee80211CipherSuite::Tkip,
    Ieee80211CipherSuite::Ccmp128,
};

/**
 * @brief List of cipher suites supported by WPA v3.
 */
constexpr std::initializer_list<Ieee80211CipherSuite> Wpa3CipherSuites{
    Ieee80211CipherSuite::Tkip,
    Ieee80211CipherSuite::Ccmp128,
    Ieee80211CipherSuite::Ccmp256,
    Ieee80211CipherSuite::Gcmp128,
    Ieee80211CipherSuite::Gcmp256,
};

/**
 * @brief Obtain a list of supported cipher suites for a given security protocol.
 *
 * @param securityProtocol The security protocol to obtain cipher suites for.
 * @return constexpr std::initializer_list<Ieee80211CipherSuite>
 */
constexpr std::initializer_list<Ieee80211CipherSuite>
WpaCipherSuites(const Ieee80211SecurityProtocol& securityProtocol)
{
    switch (securityProtocol) {
    case Ieee80211SecurityProtocol::Wpa:
        return Wpa1CipherSuites;
    case Ieee80211SecurityProtocol::Wpa2:
        return Wpa2CipherSuites;
    case Ieee80211SecurityProtocol::Wpa3:
        return Wpa3CipherSuites;
    default:
        return {};
    }
}

/**
 * @brief IEEE 802.11 BSS Types.
 *
 * Defined in IEEE 802.11-2020, Section 4.3.
 */
enum class Ieee80211BssType {
    Unknown,
    Infrastructure, // ESS
    Independent,    // IBSS
    Personal,       // PBSS
    Mesh,           // MBSS
};

/**
 * @brief Number of octets per BSSID.
 */
static constexpr auto BssidNumOctets = 6;

/**
 * @brief BSSID type.
 */
using Ieee80211Bssid = std::array<uint8_t, BssidNumOctets>;

/**
 * @brief Number of octets per MAC address.
 */
static constexpr auto MacAddressNumOctets = 6;

/**
 * @brief MAC address type.
 */
using Ieee80211MacAddress = std::array<uint8_t, MacAddressNumOctets>;

/**
 * @brief Convert a MAC address to a string.
 *
 * @param macAddress The MAC address to convert.
 * @return std::string
 */
std::string
Ieee80211MacAddressToString(const Ieee80211MacAddress& macAddress);

/**
 * @brief Parse a string into a MAC address.
 *
 * @param macAddress The MAC address string to parse.
 * @return std::optional<Ieee80211MacAddress>
 */
std::optional<Ieee80211MacAddress>
Ieee80211MacAddressFromString(std::string macAddress);

/**
 * @brief Information about a BSS.
 */
struct Ieee80211Bss
{
    Ieee80211BssType Type;
    Ieee80211Bssid Bssid;
    std::string Ssid;
};

/**
 * @brief See IEEE 802.11r-2008, page 102, 'dot11FTR0KeyHolderId'.
 */
static constexpr std::size_t Ieee80211FtR0KeyHolderIdLengthOctetsMinimum{ 1 };
static constexpr std::size_t Ieee80211FtR0KeyHolderIdLengthOctetsMaximum{ 48 };

} // namespace Microsoft::Net::Wifi

#endif // IEEE_80211_HXX
