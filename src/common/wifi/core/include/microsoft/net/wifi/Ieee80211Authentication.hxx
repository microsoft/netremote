
#ifndef IEEE_80211_AUTHENTICATION_HXX
#define IEEE_80211_AUTHENTICATION_HXX

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include <microsoft/net/Ieee8021xRadiusAuthentication.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>

namespace Microsoft::Net::Wifi
{
constexpr std::size_t Ieee80211RsnaPskLength{ 32 };
constexpr std::size_t Ieee80211RsnaPskPassphraseLengthMinimum{ 8 };
constexpr std::size_t Ieee80211RsnaPskPassphraseLengthMaximum{ 63 };

using Ieee80211RsnaPskPassphrase = std::string;
using Ieee80211RsnaPskValue = std::array<std::uint8_t, Ieee80211RsnaPskLength>;
using Ieee80211RsnaPskVariant = std::variant<Ieee80211RsnaPskPassphrase, Ieee80211RsnaPskValue>;

/**
 * @brief Encoding of a pre-shared key.
 */
enum class Ieee80211RsnaPskEncoding {
    Invalid,
    Passphrase,
    Value,
};

/**
 * @brief RSN/WPA2 (IEEE 802.11i) pre-shared key. This maps to the 802.11-2020 PSK definitions for
 * dot11RSNAConfigPSKValue and dot11RSNAConfigPSKPassPhrase.
 *
 * This class exists mostly to provide a type-safe way to handle the two possible encodings of a PSK and provide
 * syntactic sugar over using std::variant<X, Y> directly, which doesn't provid named field access.
 */
struct Ieee80211RsnaPsk :
    public Ieee80211RsnaPskVariant
{
    using Ieee80211RsnaPskVariant::Ieee80211RsnaPskVariant;

    /**
     * @brief The encoding of the pre-shared key.
     *
     * @return constexpr Ieee80211RsnaPskEncoding
     */
    constexpr Ieee80211RsnaPskEncoding
    Encoding() const noexcept
    {
        // clang-format off
        return std::holds_alternative<Ieee80211RsnaPskPassphrase>(*this)
            ? Ieee80211RsnaPskEncoding::Passphrase
            : std::holds_alternative<Ieee80211RsnaPskValue>(*this)
                ? Ieee80211RsnaPskEncoding::Value
                : Ieee80211RsnaPskEncoding::Invalid;
        // clang-format on
    }

    /**
     * @brief The pre-shared key passphrase. This is only valid when Encoding() == Ieee80211RsnaPskEncoding::Passphrase.
     *
     * @return constexpr const Ieee80211RsnaPskPassphrase&
     */
    constexpr const Ieee80211RsnaPskPassphrase&
    Passphrase() const
    {
        return std::get<Ieee80211RsnaPskPassphrase>(*this);
    }

    /**
     * @brief The pre-shared key passphrase. This is only valid when Encoding() == Ieee80211RsnaPskEncoding::Passphrase.
     *
     * @return constexpr Ieee80211RsnaPskPassphrase&
     */
    constexpr Ieee80211RsnaPskPassphrase&
    Passphrase()
    {
        return std::get<Ieee80211RsnaPskPassphrase>(*this);
    }

    /**
     * @brief The raw pre-shared key value. This is only valid when Encoding() == Ieee80211RsnaPskEncoding::Value.
     *
     * @return constexpr const Ieee80211RsnaPskValue&
     */
    constexpr const Ieee80211RsnaPskValue&
    Value() const
    {
        return std::get<Ieee80211RsnaPskValue>(*this);
    }

    /**
     * @brief The raw pre-shared key value. This is only valid when Encoding() == Ieee80211RsnaPskEncoding::Value.
     *
     * @return constexpr Ieee80211RsnaPskValue&
     */
    constexpr Ieee80211RsnaPskValue&
    Value()
    {
        return std::get<Ieee80211RsnaPskValue>(*this);
    }

    /**
     * @brief When the encoding is 'Value', returns the value as a hex-encoded string.
     *
     * @return std::string
     */
    std::string
    ToHexEncodedValue() const;
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

struct Ieee8021xAuthentication
{
    std::optional<Microsoft::Net::Ieee8021xRadiusConfiguration> Radius;
};

} // namespace Microsoft::Net::Wifi

#endif // IEEE_80211_AUTHENTICATION_HXX
