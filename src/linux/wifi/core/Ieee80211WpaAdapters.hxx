
#ifndef IEEE_80211_WPA_ADAPTERS_HXX
#define IEEE_80211_WPA_ADAPTERS_HXX

#include <cstdint>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <Wpa/ProtocolHostapd.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <microsoft/net/wifi/Ieee80211Authentication.hxx>

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
 * @brief Convert a Ieee80211AkmSuite to a WpaKeyManagement.
 *
 * @param ieee80211AkmSuite The Ieee80211AkmSuite to convert.
 * @return Wpa::WpaKeyManagement
 */
Wpa::WpaKeyManagement
Ieee80211AkmSuiteToWpaKeyManagement(Ieee80211AkmSuite ieee80211AkmSuite) noexcept;

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

/**
 * @brief Convert a Ieee80211SharedKey to a wpa credential.
 *
 * @param ieee80211SharedKey The Ieee80211SharedKey to convert.
 * @return std::vector<uint8_t>
 */
std::vector<uint8_t>
Ieee80211SharedKeyToWpaCredential(const Ieee80211SharedKey& ieee80211SharedKey) noexcept;

/**
 * @brief Convert a Ieee80211RsnaPassword to a WpaSaePassword.
 *
 * @param ieee80211RsnaPassword The Ieee80211RsnaPassword to convert.
 * @return Wpa::SaePassword
 */
Wpa::SaePassword
Ieee80211RsnaPasswordToWpaSaePassword(const Ieee80211RsnaPassword& ieee80211RsnaPassword) noexcept;
} // namespace Microsoft::Net::Wifi

#endif // IEEE_80211_WPA_ADAPTERS_HXX
