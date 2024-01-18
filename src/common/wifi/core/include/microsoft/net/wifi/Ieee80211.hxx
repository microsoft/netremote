
#ifndef IEEE_80211_HXX
#define IEEE_80211_HXX

#include <cmath>
#include <cstdint>

namespace Microsoft::Net::Wifi
{
enum class IeeeFrequencyBand {
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
inline constexpr Microsoft::Net::Wifi::IeeeFrequencyBand
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
        return Microsoft::Net::Wifi::IeeeFrequencyBand::TwoPointFourGHz;
    } else if (value == 5.0) {
        return Microsoft::Net::Wifi::IeeeFrequencyBand::FiveGHz;
    } else if (value == 6.0) {
        return Microsoft::Net::Wifi::IeeeFrequencyBand::SixGHz;
    } else {
        return Microsoft::Net::Wifi::IeeeFrequencyBand::Unknown;
    }
}

#endif // _MSC_VER

/**
 * @brief User-defined literal operator for allowing use of _MHz to specify
 * frequency band enumeration values.
 */
inline constexpr Microsoft::Net::Wifi::IeeeFrequencyBand
operator"" _MHz(unsigned long long int value) noexcept
{
    if (value == 2400) {
        return Microsoft::Net::Wifi::IeeeFrequencyBand::TwoPointFourGHz;
    } else if (value == 5000) {
        return Microsoft::Net::Wifi::IeeeFrequencyBand::FiveGHz;
    } else if (value == 6000) {
        return Microsoft::Net::Wifi::IeeeFrequencyBand::SixGHz;
    } else {
        return Microsoft::Net::Wifi::IeeeFrequencyBand::Unknown;
    }
}
} // namespace Literals

enum class IeeeProtocol {
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
enum class IeeeAuthenticationAlgorithm : uint16_t {
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
static constexpr uint8_t IeeeAkmSuiteIdReserved0 = 0;
static constexpr uint8_t IeeeAkmSuiteId8021x = 1;
static constexpr uint8_t IeeeAkmSuiteIdPsk = 2;
static constexpr uint8_t IeeeAkmSuiteIdFt8021x = 3;
static constexpr uint8_t IeeeAkmSuiteIdFtPsk = 4;
static constexpr uint8_t IeeeAkmSuiteId8021xSha256 = 5;
static constexpr uint8_t IeeeAkmSuiteIdPskSha256 = 6;
static constexpr uint8_t IeeeAkmSuiteIdTdls = 7;
static constexpr uint8_t IeeeAkmSuiteIdSae = 8;
static constexpr uint8_t IeeeAkmSuiteIdFtSae = 9;
static constexpr uint8_t IeeeAkmSuiteIdApPeerKey = 10;
static constexpr uint8_t IeeeAkmSuiteId8021xSuiteB = 11;
static constexpr uint8_t IeeeAkmSuiteId8021xSuiteB192 = 12;
static constexpr uint8_t IeeeAkmSuiteIdFt8021xSha384 = 13;
static constexpr uint8_t IeeeAkmSuiteIdFilsSha256 = 14;
static constexpr uint8_t IeeeAkmSuiteIdFilsSha384 = 15;
static constexpr uint8_t IeeeAkmSuiteIdFtFilsSha256 = 16;
static constexpr uint8_t IeeeAkmSuiteIdFtFilsSha384 = 17;
static constexpr uint8_t IeeeAkmSuiteIdOwe = 18;
static constexpr uint8_t IeeeAkmSuiteIdFtPskSha384 = 19;
static constexpr uint8_t IeeeAkmSuiteIdPskSha384 = 20;

/**
 * @brief IEEE 802.11 Authentication and Key Management (AKM) Suites.
 *
 * Defined in IEEE 802.11-2020, Section 9.4.2.24.3, Table 9-151.
 */
enum class IeeeAkmSuite : uint32_t {
    Reserved0 = MakeIeee80211Suite(IeeeAkmSuiteIdReserved0),
    Ieee8021x = MakeIeee80211Suite(IeeeAkmSuiteId8021x),
    Psk = MakeIeee80211Suite(IeeeAkmSuiteIdPsk),
    Ft8021x = MakeIeee80211Suite(IeeeAkmSuiteIdFt8021x),
    FtPsk = MakeIeee80211Suite(IeeeAkmSuiteIdFtPsk),
    Ieee8021xSha256 = MakeIeee80211Suite(IeeeAkmSuiteId8021xSha256),
    PskSha256 = MakeIeee80211Suite(IeeeAkmSuiteIdPskSha256),
    Tdls = MakeIeee80211Suite(IeeeAkmSuiteIdTdls),
    Sae = MakeIeee80211Suite(IeeeAkmSuiteIdSae),
    FtSae = MakeIeee80211Suite(IeeeAkmSuiteIdFtSae),
    ApPeerKey = MakeIeee80211Suite(IeeeAkmSuiteIdApPeerKey),
    Ieee8021xSuiteB = MakeIeee80211Suite(IeeeAkmSuiteId8021xSuiteB),
    Ieee8011xSuiteB192 = MakeIeee80211Suite(IeeeAkmSuiteId8021xSuiteB192),
    Ft8021xSha384 = MakeIeee80211Suite(IeeeAkmSuiteIdFt8021xSha384),
    FilsSha256 = MakeIeee80211Suite(IeeeAkmSuiteIdFilsSha256),
    FilsSha284 = MakeIeee80211Suite(IeeeAkmSuiteIdFilsSha384),
    FtFilsSha256 = MakeIeee80211Suite(IeeeAkmSuiteIdFtFilsSha256),
    FtFilsSha384 = MakeIeee80211Suite(IeeeAkmSuiteIdFtFilsSha384),
    Owe = MakeIeee80211Suite(IeeeAkmSuiteIdOwe),
    FtPskSha384 = MakeIeee80211Suite(IeeeAkmSuiteIdFtPskSha384),
    PskSha384 = MakeIeee80211Suite(IeeeAkmSuiteIdPskSha384),
};

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
enum class IeeeCipherSuite : uint32_t {
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

} // namespace Microsoft::Net::Wifi

#endif // IEEE_80211_HXX
