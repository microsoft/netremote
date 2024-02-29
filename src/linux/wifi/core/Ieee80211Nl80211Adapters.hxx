
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
Ieee80211CipherSuite
Nl80211CipherSuiteToIeee80211CipherSuite(uint32_t nl80211CipherSuite) noexcept;

Ieee80211AkmSuite
Nl80211AkmSuiteToIeee80211AkmSuite(uint32_t nl80211AkmSuite) noexcept;

Ieee80211FrequencyBand
Nl80211BandToIeee80211FrequencyBand(nl80211_band nl80211Band) noexcept;

std::vector<Ieee80211Protocol>
Nl80211WiphyToIeee80211Protocols(const Microsoft::Net::Netlink::Nl80211::Nl80211Wiphy& nl80211Wiphy);

Wpa::HostapdHwMode
IeeeProtocolToHostapdHwMode(Ieee80211Protocol ieeeProtocol);

std::string
HostapdHwModeToPropertyValue(Wpa::HostapdHwMode hwMode);

std::string_view
IeeeFrequencyBandToHostapdBand(Ieee80211FrequencyBand ieeeFrequencyBand);

} // namespace Microsoft::Net::Wifi

#endif // IEEE_80211_NL80211_ADAPTERS_HXX
