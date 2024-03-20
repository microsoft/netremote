
#ifndef IEEE_80211_NL80211_ADAPTERS_HXX
#define IEEE_80211_NL80211_ADAPTERS_HXX

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include <linux/nl80211.h>
#include <microsoft/net/netlink/nl80211/Netlink80211Wiphy.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Convert the specific nl80211 wpa version to the equivalent Ieee80211SecurityProtocol.
 *
 * @param nl80211WpaVersion The nl80211 wpa version to convert.
 * @return Ieee80211SecurityProtocol
 */
Ieee80211SecurityProtocol
Nl80211WpaVersionToIeee80211SecurityProtocol(nl80211_wpa_versions nl80211WpaVersion) noexcept;

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
 * @brief Obtain a list of Ieee80211PhyTypes from a Nl80211Wiphy.
 *
 * @param nl80211Wiphy The Nl80211Wiphy to obtain the protocols from.
 * @return std::vector<Ieee80211PhyType>
 */
std::vector<Ieee80211PhyType>
Nl80211WiphyToIeee80211PhyTypes(const Microsoft::Net::Netlink::Nl80211::Nl80211Wiphy& nl80211Wiphy);
} // namespace Microsoft::Net::Wifi

#endif // IEEE_80211_NL80211_ADAPTERS_HXX
