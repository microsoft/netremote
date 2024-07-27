
#ifndef ACCESS_POINT_ATTRIBUTES_HXX
#define ACCESS_POINT_ATTRIBUTES_HXX

#include <istream>
#include <optional>
#include <string>
#include <unordered_map>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Container to hold static attributes about an access point.
 */
struct AccessPointAttributes
{
    AccessPointAttributes() = default;

    /**
     * @brief Attempt to deserialize a JSON string into a map of access point attributes.
     *
     * @param json The JSON input string to parse.
     * @return std::optional<std::unordered_map<std::string, AccessPointAttributes>>
     */
    static std::optional<std::unordered_map<std::string, AccessPointAttributes>>
    TryParseJson(const std::string& json);

    /**
     * @brief Attempt to deserialize a JSON stream into a map of access point attributes.
     *
     * @param json The JSON input stream to parse.
     * @return std::optional<std::unordered_map<std::string, AccessPointAttributes>>
     */
    static std::optional<std::unordered_map<std::string, AccessPointAttributes>>
    TryParseJson(std::istream& json);

    std::unordered_map<std::string, std::string> Properties{};
};
} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_ATTRIBUTES_HXX
