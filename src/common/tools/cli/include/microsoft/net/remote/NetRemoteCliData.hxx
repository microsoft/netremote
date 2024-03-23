
#ifndef NET_REMOTE_CLI_DATA_HXX
#define NET_REMOTE_CLI_DATA_HXX

#include <optional>
#include <string>
#include <vector>

#include <microsoft/net/remote/protocol/NetRemoteProtocol.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>

namespace Microsoft::Net::Remote
{
struct NetRemoteCli;

/**
 * @brief Data associated with the NetRemote CLI. This is used to store command-line arguments and other data associated
 * with CLI configuration and operation.
 */
struct NetRemoteCliData
{
    std::string ServerAddress{ Protocol::NetRemoteProtocol::AddressDefault };
    std::optional<bool> DetailedOutput;

    std::string WifiAccessPointId{};
    std::string WifiAccessPointSsid{};
    Microsoft::Net::Wifi::Ieee80211PhyType WifiAccessPointPhyType{ Microsoft::Net::Wifi::Ieee80211PhyType::Unknown };
    std::vector<Microsoft::Net::Wifi::Ieee80211FrequencyBand> WifiAccessPointFrequencyBands{};
    std::vector<Microsoft::Net::Wifi::Ieee80211AuthenticationAlgorithm> WifiAccessPointAuthenticationAlgorithms{};
    std::vector<Microsoft::Net::Wifi::Ieee80211AkmSuite> WifiAccessPointAkmSuites{};
};
} // namespace Microsoft::Net::Remote

#endif // NET_REMOTE_CLI_DATA_HXX
