
#ifndef IEEE_80211_WPA_ADAPTERS_HXX
#define IEEE_80211_WPA_ADAPTERS_HXX

#include <string>
#include <string_view>
#include <unordered_map>

#include <Wpa/ProtocolHostapd.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Convert a WPA "hwmode" value from a Ieee80211PhyType value.
 *
 * @param ieeePhyType The Ieee80211PhyType value to convert.
 * @return Wpa::HostapdHwMode
 */
Wpa::HostapdHwMode
IeeePhyTypeToHostapdHwMode(Ieee80211PhyType ieeePhyType) noexcept;

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

/**
 * @brief Convert a map of Ieee80211SecurityProtocol to list of Ieee80211CipherSuite to a map of WpaProtocol to list of WpaCipher.
 * 
 * @param ieee80211CipherSuites The map of Ieee80211SecurityProtocol to list of Ieee80211CipherSuite to convert.
 * @return std::unordered_map<Wpa::WpaProtocol, Wpa::WpaCipher> 
 */
std::unordered_map<Wpa::WpaProtocol, Wpa::WpaCipher>
Ieee80211CipherSuitesToWpaCipherSuites(const std::unordered_map<Ieee80211SecurityProtocol, std::vector<Ieee80211CipherSuite>>& ieee80211CipherSuites) noexcept;
} // namespace Microsoft::Net::Wifi

#endif // IEEE_80211_WPA_ADAPTERS_HXX
