
#ifndef ACCESS_POINT_ATTRIBUTES_JSON_SERIALIZATION_HXX
#define ACCESS_POINT_ATTRIBUTES_JSON_SERIALIZATION_HXX

#include <format>
#include <optional>
#include <string>
#include <unordered_map>

#include <microsoft/net/wifi/AccessPointAttributes.hxx>
#include <nlohmann/json.hpp>
#include <plog/Log.h>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Serialize access point attributes to JSON.
 *
 * @param accessPointAttributesJson The JSON object to hold the serialized data.
 * @param accessPointAttributes The access point attributes to serialize.
 */
void
to_json(nlohmann::json& accessPointAttributesJson, const AccessPointAttributes& accessPointAttributes);

/**
 * @brief Deserialize access point attributes from JSON.
 *
 * @param accessPointAttributesJson The JSON object to deserialize.
 * @param accessPointAttributes The access point attributes to populate.
 */
void
from_json(const nlohmann::json& accessPointAttributesJson, AccessPointAttributes& accessPointAttributes);

/**
 * @brief Parse access point attributes from a JSON input type.
 *
 * @tparam InputType The input type holding the JSON to parse. This must be one of the types supported by
 * nlohmann::json::parse (input stream, string, iterator pair, contiguous container, character array).
 * @param json The JSON input to parse.
 * @return std::optional<std::unordered_map<std::string, AccessPointAttributes>>
 */
template <typename InputType>
std::optional<std::unordered_map<std::string, AccessPointAttributes>>
ParseAccessPointAttributesFromJson(InputType& json)
{
    std::unordered_map<std::string, AccessPointAttributes> accessPointAttributesMap;
    nlohmann::json accessPointAttributesJson;

    try {
        accessPointAttributesJson = nlohmann::json::parse(json);
        accessPointAttributesJson.get_to(accessPointAttributesMap);
    } catch (const nlohmann::json::parse_error& e) {
        LOGE << std::format("Failed to parse access point access point attributes JSON: {}", e.what());
        return std::nullopt;
    }

    return accessPointAttributesMap;
}

} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_ATTRIBUTES_JSON_SERIALIZATION_HXX
