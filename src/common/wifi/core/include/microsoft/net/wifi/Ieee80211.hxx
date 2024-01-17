
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

enum class IeeeAuthenticationAlgorithm {
    Unknown,
    Open,
    SharedKey,
    Wpa,
    WpaPsk,
    WpaNone,
    Rsna,
    RsnaPsk,
    Wpa3,
    Wpa3Enterprise192,
    Wpa3Enterprise,
    Sae,
    Owe,
};

constexpr auto Wpa3Enterprise192 = IeeeAuthenticationAlgorithm::Wpa3;
constexpr auto Wpa3Personal = IeeeAuthenticationAlgorithm::Sae;

/**
 * @brief OUI for IEEE 802.11 organization.
 */
static constexpr uint32_t OuiIeee80211 = 0x00000FAC;

/**
 * @brief OUI placeholder for signaling invalid values.
 */
static constexpr uint32_t OuiInvalid = 0x00FFFFFF;

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
 * @brief Helper function to construct an IEEE cipher suite value.
 *
 * The OUT is 3 octets and the suite ID is 1 octet.
 *
 * @param oui The OUI value.
 * @param suiteId The suite ID value.
 * @return constexpr uint32_t
 */
constexpr uint32_t
MakeIeeeCipherSuite(uint32_t oui, uint8_t suiteId)
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
MakeIeee80211CipherSuite(uint8_t suiteId)
{
    return MakeIeeeCipherSuite(OuiIeee80211, suiteId);
}

/**
 * @brief IEEE 802.11 Cipher Suites.
 *
 * Defined in IEEE 802.11-2020, Section 9.4.2.24.2, Table 9-149.
 */
enum class IeeeCipherSuite : uint32_t {
    Unknown = MakeIeeeCipherSuite(OuiInvalid, 0),
    BipCmac128 = MakeIeee80211CipherSuite(Ieee80211CipherSuiteIdBipCmac128),
    BipCmac256 = MakeIeee80211CipherSuite(Ieee80211CipherSuiteIdBipCmac256),
    BipGmac128 = MakeIeee80211CipherSuite(Ieee80211CipherSuiteIdBipGmac128),
    BipGmac256 = MakeIeee80211CipherSuite(Ieee80211CipherSuiteIdBipGmac256),
    Ccmp128 = MakeIeee80211CipherSuite(Ieee80211CipherSuiteCcmp128),
    Ccmp256 = MakeIeee80211CipherSuite(Ieee80211CipherSuiteIdCcmp256),
    Gcmp128 = MakeIeee80211CipherSuite(Ieee80211CipherSuiteIdGcmp128),
    Gcmp256 = MakeIeee80211CipherSuite(Ieee80211CipherSuiteIdGcmp256),
    GroupAddressesTrafficNotAllowed = MakeIeee80211CipherSuite(Ieee80211CipherSuiteIdGroupAddressTrafficNotAllowed),
    Tkip = MakeIeee80211CipherSuite(Ieee80211CipherSuiteIdTkip),
    UseGroup = MakeIeee80211CipherSuite(Ieee80211CipherSuiteIdUseGroup),
    Wep104 = MakeIeee80211CipherSuite(Ieee80211CipherSuiteIdWep104),
    Wep40 = MakeIeee80211CipherSuite(Ieee80211CipherSuiteIdWep40),
};

} // namespace Microsoft::Net::Wifi

#endif // IEEE_80211_HXX
