
#ifndef NET_REMOTE_CLI_DATA_HXX
#define NET_REMOTE_CLI_DATA_HXX

#include <optional>
#include <string>
#include <tuple>
#include <vector>

#include <microsoft/net/remote/protocol/NetRemoteProtocol.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <microsoft/net/wifi/Ieee80211Authentication.hxx>

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
    std::string WifiAccessPointPskPassphrase;
    std::string WifiAccessPointPskHex;
    std::vector<std::tuple<std::string, std::optional<std::string>, std::optional<std::string>>> WifiAccessPointSaePasswords{};
    std::vector<Microsoft::Net::Wifi::Ieee80211FrequencyBand> WifiAccessPointFrequencyBands{};
    std::vector<Microsoft::Net::Wifi::Ieee80211AuthenticationAlgorithm> WifiAccessPointAuthenticationAlgorithms{};
    std::vector<Microsoft::Net::Wifi::Ieee80211AkmSuite> WifiAccessPointAkmSuites{};
    Microsoft::Net::Wifi::Ieee80211PhyType WifiAccessPointPhyType{ Microsoft::Net::Wifi::Ieee80211PhyType::Unknown };
    Microsoft::Net::Wifi::Ieee80211Authentication8021x WifiAccessPointAuthentication8021x{};
};
} // namespace Microsoft::Net::Remote

#endif // NET_REMOTE_CLI_DATA_HXX
