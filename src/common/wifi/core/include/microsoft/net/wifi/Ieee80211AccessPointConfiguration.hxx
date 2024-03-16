
#ifndef IEEE_80211_ACCESS_POINT_CONFIGURATION
#define IEEE_80211_ACCESS_POINT_CONFIGURATION

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <microsoft/net/wifi/Ieee80211.hxx>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Configuration for an IEEE 802.11 access point.
 */
struct Ieee80211AccessPointConfiguration
{
    std::optional<std::string> Ssid;
    std::optional<Ieee80211Bssid> Bssid;
    std::optional<Ieee80211PhyType> PhyType;
    std::unordered_map<Ieee80211SecurityProtocol, Ieee80211CipherSuite> PairwiseCipherSuites;
    std::vector<Ieee80211AuthenticationAlgorithm> AuthenticationAlgorithms;
    std::vector<Ieee80211FrequencyBand> FrequencyBands;
};
} // namespace Microsoft::Net::Wifi

#endif // IEEE_80211_ACCESS_POINT_CONFIGURATION
