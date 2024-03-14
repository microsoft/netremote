
#include <algorithm>
#include <cstdint>
#include <format>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

#include <Wpa/ProtocolHostapd.hxx>
#include <linux/nl80211.h>
#include <magic_enum.hpp>
#include <microsoft/net/netlink/nl80211/Netlink80211Wiphy.hxx>
#include <microsoft/net/wifi/IAccessPointController.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <plog/Log.h>

#include "Ieee80211Nl80211Adapters.hxx"

using Microsoft::Net::Netlink::Nl80211::Nl80211Wiphy;

namespace Microsoft::Net::Wifi
{
Ieee80211CipherSuite
Nl80211CipherSuiteToIeee80211CipherSuite(uint32_t nl80211CipherSuite) noexcept
{
    return static_cast<Ieee80211CipherSuite>(nl80211CipherSuite);
}

Ieee80211AkmSuite
Nl80211AkmSuiteToIeee80211AkmSuite(uint32_t nl80211AkmSuite) noexcept
{
    return static_cast<Ieee80211AkmSuite>(nl80211AkmSuite);
}

Ieee80211FrequencyBand
Nl80211BandToIeee80211FrequencyBand(nl80211_band nl80211Band) noexcept
{
    switch (nl80211Band) {
    case NL80211_BAND_2GHZ:
        return Ieee80211FrequencyBand::TwoPointFourGHz;
    case NL80211_BAND_5GHZ:
        return Ieee80211FrequencyBand::FiveGHz;
    case NL80211_BAND_60GHZ:
        return Ieee80211FrequencyBand::SixGHz;
    default:
        return Ieee80211FrequencyBand::Unknown;
    }
}

std::vector<Ieee80211Protocol>
Nl80211WiphyToIeee80211Protocols(const Nl80211Wiphy& nl80211Wiphy)
{
    // Ieee80211 B & G are always supported.
    std::vector<Ieee80211Protocol> protocols{
        Ieee80211Protocol::B,
        Ieee80211Protocol::G,
    };

    for (const auto& band : std::views::values(nl80211Wiphy.Bands)) {
        if (band.HtCapabilities != 0) {
            protocols.push_back(Ieee80211Protocol::N);
        }
        if (band.VhtCapabilities != 0) {
            protocols.push_back(Ieee80211Protocol::AC);
        }
        // TODO: once Nl80211WiphyBand is updated to support HE (AX) and EHT (BE), add them here.
    }

    // Remove duplicates.
    std::ranges::sort(protocols);
    protocols.erase(std::ranges::begin(std::ranges::unique(protocols)), std::ranges::end(protocols));

    return protocols;
}

} // namespace Microsoft::Net::Wifi
