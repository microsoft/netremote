
#ifndef IEEE_80211_HXX
#define IEEE_80211_HXX

#include <cmath>

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

enum class IeeeCipherAlgorithm {
    Unknown,
    None,
    Wep,
    Wep40,
    Wep104,
    Tkip,
    BipCmac128,
    BipGmac128,
    BipGmac256,
    BipCmac256,
    Gcmp128,
    Gcmp256,
    Ccmp256,
    WpaUseGroup,
};

constexpr auto Bip = IeeeCipherAlgorithm::BipCmac128;
constexpr auto Gcmp = IeeeCipherAlgorithm::Gcmp128;
constexpr auto RsnUseGroup = IeeeCipherAlgorithm::WpaUseGroup;

} // namespace Microsoft::Net::Wifi

#endif // IEEE_80211_HXX
