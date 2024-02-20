
#ifndef WPA_KEY_VALUE_PAIR_HXX
#define WPA_KEY_VALUE_PAIR_HXX

#include <optional>
#include <string_view>

#include <Wpa/ProtocolWpa.hxx>
#include <notstd/Utility.hxx>

namespace Wpa
{
/**
 * @brief Describes the presence of a property in a WPA control protocol
 * response.
 */
enum class WpaValuePresence : bool {
    Required = true,
    Optional = false,
};

/**
 * @brief Represents a key-value pair in the WPA control protocol. Most commands
 * output lists of data as key-value pairs with a '=' delimiter.
 *
 * This class helps parse and collect such pairs.
 */
struct WpaKeyValuePair
{
    /**
     * @brief The delimeter separating keys from values.
     */
    static constexpr auto KeyDelimiter = ProtocolWpa::KeyValueDelimiter;

    /**
     * @brief Construct a new WpaKeyValue Pair object.
     *
     * @param key The key of the property.
     * @param presence Whether the property is required or optional.
     * @param isIndexed Whether the property is indexed (i.e. has a numeric suffix in its key name).
     */
    constexpr WpaKeyValuePair(std::string_view key, WpaValuePresence presence, bool isIndexed = false) :
        Key(key),
        IsRequired(notstd::to_underlying(presence)),
        IsIndexed(isIndexed)
    {
    }

    std::string_view Key;
    std::optional<std::string_view> Value;
    bool IsRequired{ true };
    bool IsIndexed{ false };
};
} // namespace Wpa

#endif // WPA_KEY_VALUE_PAIR_HXX
