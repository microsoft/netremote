
#ifndef IEEE_80211_ACCESS_POINT_CONFIGURATION
#define IEEE_80211_ACCESS_POINT_CONFIGURATION

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <microsoft/net/wifi/Ieee80211.hxx>
#include <microsoft/net/wifi/Ieee80211Authentication.hxx>

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
    std::vector<Ieee80211AkmSuite> AkmSuites;
    std::unordered_map<Ieee80211SecurityProtocol, std::vector<Ieee80211CipherSuite>> PairwiseCipherSuites;
    std::vector<Ieee80211AuthenticationAlgorithm> AuthenticationAlgorithms;
    std::vector<Ieee80211FrequencyBand> FrequencyBands;
    Ieee80211AuthenticationData AuthenticationData;
};
} // namespace Microsoft::Net::Wifi

#endif // IEEE_80211_ACCESS_POINT_CONFIGURATION
