
#ifndef IEEE_80211_ACCESS_POINT_CAPABILITIES_HXX
#define IEEE_80211_ACCESS_POINT_CAPABILITIES_HXX

#include <vector>

#include <microsoft/net/wifi/Ieee80211.hxx>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Describes the capabilities of an IEEE 802.11 access point.
 */
struct Ieee80211AccessPointCapabilities
{
    std::vector<IeeeProtocol> Protocols;
    std::vector<IeeeFrequencyBand> FrequencyBands;
    std::vector<IeeeAuthenticationAlgorithm> AuthenticationAlgorithms;
    std::vector<IeeeCipherSuite> EncryptionAlgorithms;
};
} // namespace Microsoft::Net::Wifi

#endif // IEEE_80211_ACCESS_POINT_CAPABILITIES_HXX