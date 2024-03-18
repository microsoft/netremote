
#include <algorithm>
#include <cstdint>
#include <format>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

#include <linux/nl80211.h>
#include <magic_enum.hpp>
#include <microsoft/net/netlink/nl80211/Ieee80211Nl80211Adapters.hxx>
#include <microsoft/net/netlink/nl80211/Netlink80211Wiphy.hxx>
#include <microsoft/net/wifi/IAccessPointController.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>

#include <plog/Log.h>

using Microsoft::Net::Netlink::Nl80211::Nl80211Wiphy;

namespace Microsoft::Net::Wifi
{
Ieee80211SecurityProtocol
Nl80211WpaVersionToIeee80211SecurityProtocol(nl80211_wpa_versions nl80211WpaVersion) noexcept
{
    switch (nl80211WpaVersion) {
    case NL80211_WPA_VERSION_1:
        return Ieee80211SecurityProtocol::Wpa;
    case NL80211_WPA_VERSION_2:
        return Ieee80211SecurityProtocol::Wpa2;
    case NL80211_WPA_VERSION_3:
        return Ieee80211SecurityProtocol::Wpa3;
    default:
        return Ieee80211SecurityProtocol::Unknown;
    }
}

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

std::vector<Ieee80211PhyType>
Nl80211WiphyToIeee80211PhyTypes(const Nl80211Wiphy& nl80211Wiphy)
{
    // Ieee80211 B & G are always supported.
    std::vector<Ieee80211PhyType> phyTypes{
        Ieee80211PhyType::B,
        Ieee80211PhyType::G,
    };

    for (const auto& band : std::views::values(nl80211Wiphy.Bands)) {
        if (band.HtCapabilities != 0) {
            phyTypes.push_back(Ieee80211PhyType::N);
        }
        if (band.VhtCapabilities != 0) {
            phyTypes.push_back(Ieee80211PhyType::AC);
        }
        // TODO: once Nl80211WiphyBand is updated to support HE (AX) and EHT (BE), add them here.
    }

    // Remove duplicates.
    std::ranges::sort(phyTypes);
    phyTypes.erase(std::ranges::begin(std::ranges::unique(phyTypes)), std::ranges::end(phyTypes));

    return phyTypes;
}

} // namespace Microsoft::Net::Wifi
