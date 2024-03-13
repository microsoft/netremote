
#ifndef IEEE_80211_WPA_ADAPTERS_HXX
#define IEEE_80211_WPA_ADAPTERS_HXX

#include <string>
#include <string_view>

#include <Wpa/ProtocolHostapd.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Convert a WPA "hwmode" value from a Ieee80211Protocol value.
 *
 * @param ieeeProtocol The Ieee80211Protocol value to convert.
 * @return Wpa::HostapdHwMode
 */
Wpa::HostapdHwMode
IeeeProtocolToHostapdHwMode(Ieee80211Protocol ieeeProtocol) noexcept;

/**
 * @brief Get a string representation of a HostapdHwMode.
 *
 * @param hwMode The HostapdHwMode value to convert.
 * @return std::string
 */
std::string
HostapdHwModeToPropertyValue(Wpa::HostapdHwMode hwMode) noexcept;

/**
 * @brief Get a string representation of a Ieee80211FrequencyBand.
 *
 * @param ieeeFrequencyBand The Ieee80211FrequencyBand value to convert.
 * @return std::string_view
 */
std::string_view
IeeeFrequencyBandToHostapdBand(Ieee80211FrequencyBand ieeeFrequencyBand) noexcept;

/**
 * @brief Convert a Ieee80211AuthenticationAlgorithm to a WpaAuthenticationAlgorithm.
 *
 * @param ieee80211AuthenticationAlgorithm The Ieee80211AuthenticationAlgorithm to convert.
 * @return Wpa::WpaAuthenticationAlgorithm
 */
Wpa::WpaAuthenticationAlgorithm
Ieee80211AuthenticationAlgorithmToWpaAuthenticationAlgorithm(Ieee80211AuthenticationAlgorithm ieee80211AuthenticationAlgorithm) noexcept;
} // namespace Microsoft::Net::Wifi

#endif // IEEE_80211_WPA_ADAPTERS_HXX
