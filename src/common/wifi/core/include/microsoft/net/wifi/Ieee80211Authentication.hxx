
#ifndef IEEE_80211_AUTHENTICATION_HXX
#define IEEE_80211_AUTHENTICATION_HXX

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include <microsoft/net/wifi/Ieee80211.hxx>

namespace Microsoft::Net::Wifi
{
constexpr std::size_t Ieee80211RsnaPskLength{ 32 };
constexpr std::size_t Ieee80211RsnaPskSecretLength{ Ieee80211RsnaPskLength * 2 }; // 2 hex chars per byte
constexpr std::size_t Ieee80211RsnaPskPassphraseLengthMinimum{ 8 };
constexpr std::size_t Ieee80211RsnaPskPassphraseLengthMaximum{ 63 };

using Ieee80211RsnaPskPassphrase = std::string;
using Ieee80211RsnaPskValue = std::array<std::uint8_t, Ieee80211RsnaPskLength>;
using Ieee80211RsnaPskSecret = std::array<char, Ieee80211RsnaPskSecretLength>;
using Ieee80211RsnaPskVariant = std::variant<Ieee80211RsnaPskPassphrase, Ieee80211RsnaPskSecret>;

/**
 * @brief Encoding of a pre-shared key.
 *
 * The format when the type is 'Secret' is dependent on the context.
 */
enum class Ieee80211RsnaPskEncoding {
    Passphrase,
    Secret,
};

struct Ieee80211RsnaPsk :
    public Ieee80211RsnaPskVariant
{
    using Ieee80211RsnaPskVariant::Ieee80211RsnaPskVariant;

    constexpr Ieee80211RsnaPskEncoding
    Encoding() const noexcept
    {
        return std::holds_alternative<Ieee80211RsnaPskPassphrase>(*this)
            ? Ieee80211RsnaPskEncoding::Passphrase
            : Ieee80211RsnaPskEncoding::Secret;
    }

    constexpr const Ieee80211RsnaPskPassphrase&
    Passphrase() const
    {
        return std::get<Ieee80211RsnaPskPassphrase>(*this);
    }

    constexpr Ieee80211RsnaPskPassphrase&
    Passphrase()
    {
        return std::get<Ieee80211RsnaPskPassphrase>(*this);
    }

    constexpr const Ieee80211RsnaPskSecret&
    Secret() const
    {
        return std::get<Ieee80211RsnaPskSecret>(*this);
    }

    constexpr Ieee80211RsnaPskSecret&
    Secret()
    {
        return std::get<Ieee80211RsnaPskSecret>(*this);
    }
};

struct Ieee80211RsnaPassword
{
    std::string Credential;
    std::optional<std::string> PasswordId;
    std::optional<Ieee80211MacAddress> PeerMacAddress;
};

struct Ieee80211AuthenticationDataPsk
{
    Ieee80211RsnaPsk Psk;
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
