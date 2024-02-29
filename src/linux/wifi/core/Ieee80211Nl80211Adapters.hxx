
#ifndef IEEE_80211_NL80211_ADAPTERS_HXX
#define IEEE_80211_NL80211_ADAPTERS_HXX

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include <Wpa/ProtocolHostapd.hxx>
#include <linux/nl80211.h>
#include <microsoft/net/netlink/nl80211/Netlink80211Wiphy.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Convert the specified nl80211 cipher suite value to the equivalent Ieee80211CipherSuite.
 *
 * @param nl80211CipherSuite The nl80211 cipher suite value to convert.
 * @return Ieee80211CipherSuite
 */
Ieee80211CipherSuite
Nl80211CipherSuiteToIeee80211CipherSuite(uint32_t nl80211CipherSuite) noexcept;

/**
 * @brief Convert the specified nl80211 akm suite value to the equivalent Ieee80211AkmSuite.
 *
 * @param nl80211AkmSuite The nl80211 akm suite value to convert.
 * @return Ieee80211AkmSuite
 */
Ieee80211AkmSuite
Nl80211AkmSuiteToIeee80211AkmSuite(uint32_t nl80211AkmSuite) noexcept;

/**
 * @brief Convert the specified nl80211_band value to the equivalent Ieee80211FrequencyBand.
 *
 * @param nl80211Band The nl80211_band value to convert.
 * @return Ieee80211FrequencyBand
 */
Ieee80211FrequencyBand
Nl80211BandToIeee80211FrequencyBand(nl80211_band nl80211Band) noexcept;

/**
 * @brief Obtain a list of Ieee80211Protocols from a Nl80211Wiphy.
 *
 * @param nl80211Wiphy The Nl80211Wiphy to obtain the protocols from.
 * @return std::vector<Ieee80211Protocol>
 */
std::vector<Ieee80211Protocol>
Nl80211WiphyToIeee80211Protocols(const Microsoft::Net::Netlink::Nl80211::Nl80211Wiphy& nl80211Wiphy);

/**
 * @brief Convert a WPA "hwmode" value from a Ieee80211Protocol value.
 *
 * @param ieeeProtocol The Ieee80211Protocol value to convert.
 * @return Wpa::HostapdHwMode
 */
Wpa::HostapdHwMode
IeeeProtocolToHostapdHwMode(Ieee80211Protocol ieeeProtocol);

/**
 * @brief Get a string representation of a HostapdHwMode.
 *
 * @param hwMode The HostapdHwMode value to convert.
 * @return std::string
 */
std::string
HostapdHwModeToPropertyValue(Wpa::HostapdHwMode hwMode);

/**
 * @brief Get a string representation of a Ieee80211FrequencyBand.
 *
 * @param ieeeFrequencyBand The Ieee80211FrequencyBand value to convert.
 * @return std::string_view
 */
std::string_view
IeeeFrequencyBandToHostapdBand(Ieee80211FrequencyBand ieeeFrequencyBand);

} // namespace Microsoft::Net::Wifi

#endif // IEEE_80211_NL80211_ADAPTERS_HXX
