
#ifndef IEEE_80211_AUTHENTICATION_HXX
#define IEEE_80211_AUTHENTICATION_HXX

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <microsoft/net/wifi/Ieee80211.hxx>

namespace Microsoft::Net::Wifi
{
constexpr std::size_t Ieee80211PskLengthMinimum{ 8 };
constexpr std::size_t Ieee80211PskLengthMaximum{ 63 };

struct Ieee80211SharedKey
{
    std::vector<std::uint8_t> Data;
};

struct Ieee80211RsnaPassword
{
    Ieee80211SharedKey Credential;
    std::optional<std::string> PasswordId;
    std::optional<Ieee80211MacAddress> PeerMacAddress;
};

struct Ieee80211AuthenticationDataPsk
{
    Ieee80211SharedKey Key;
};

struct Ieee80211AuthenticationDataSae
{
    std::vector<Ieee80211RsnaPassword> Passwords;
};

struct Ieee80211AuthenticationData
{
    std::optional<Ieee80211AuthenticationDataPsk> Psk;
    std::optional<Ieee80211AuthenticationDataSae> Sae;
};

} // namespace Microsoft::Net::Wifi

#endif // IEEE_80211_AUTHENTICATION_HXX
