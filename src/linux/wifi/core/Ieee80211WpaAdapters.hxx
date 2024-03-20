
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
 * @brief Convert a Ieee80211SecurityProtocol to a WpaSecurityProtocol.
 * 
 * @param ieee80211SecurityProtocol The Ieee80211SecurityProtocol to convert.
 * @return Wpa::WpaSecurityProtocol 
 */
Wpa::WpaSecurityProtocol
Ieee80211SecurityProtocolToWpaSecurityProtocol(Ieee80211SecurityProtocol ieee80211SecurityProtocol) noexcept;

/**
 * @brief Convert a Ieee80211CipherSuite to a WpaCipher.
 * 
 * @param ieee80211CipherSuite The Ieee80211CipherSuite to convert.
 * @return Wpa::WpaCipher 
 */
Wpa::WpaCipher
Ieee80211CipherSuiteToWpaCipher(Ieee80211CipherSuite ieee80211CipherSuite) noexcept;

/**
 * @brief Convert a map of Ieee80211SecurityProtocol to list of Ieee80211CipherSuite to a map of WpaSecurityProtocol to list of WpaCipher.
 * 
 * @param ieee80211CipherSuiteConfigurations The map of Ieee80211SecurityProtocol to list of Ieee80211CipherSuite to convert.
 * @return std::unordered_map<Wpa::WpaSecurityProtocol, std::vector<Wpa::WpaCipher>> 
 */
std::unordered_map<Wpa::WpaSecurityProtocol, std::vector<Wpa::WpaCipher>>
Ieee80211CipherSuitesToWpaCipherSuites(const std::unordered_map<Ieee80211SecurityProtocol, std::vector<Ieee80211CipherSuite>>& ieee80211CipherSuiteConfigurations) noexcept;
} // namespace Microsoft::Net::Wifi

#endif // IEEE_80211_WPA_ADAPTERS_HXX
