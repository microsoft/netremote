
#ifndef WPA_KEY_VALUE_PAIR_HXX
#define WPA_KEY_VALUE_PAIR_HXX

#include <optional>
#include <string_view>

namespace Wpa
{
enum class WpaValuePresence : bool
{
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
    static constexpr auto KeyDelimiter = '=';

    /**
     * @brief Construct a new WpaKeyValue Pair object.
     * 
     * @param key The key of the property. 
     * @param presence Whether the property is required or optional.
     */
    constexpr WpaKeyValuePair(std::string_view key, WpaValuePresence presence = WpaValuePresence::Required);

    /**
      * Parses the input string and attempts to resolve the property value,
     * assigning it to the Value member if found. Note that this function does
     * not parse the value itself, it only resolves the value location in the
     * input string, making it available for later parsing by derived classes
     * that know its type/structure.
     * 
     * The input string is expected to contain a property of the form:
     * key=value, as is encoded by the WPA control protocol.    * @brief 
     * 
     * @param input 
     * @return std::optional<std::string_view> 
     */
    std::optional<std::string_view> TryParseValue(std::string_view input);

    std::string_view Key;
    std::optional<std::string_view> Value;
    bool IsRequired{ true };
};
} // namespace Wpa

#endif // WPA_KEY_VALUE_PAIR_HXX
