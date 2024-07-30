
#include <istream>
#include <optional>
#include <string>
#include <unordered_map>

#include <microsoft/net/wifi/AccessPointAttributes.hxx>
#include <microsoft/net/wifi/AccessPointAttributesJsonSerialization.hxx>

using namespace Microsoft::Net::Wifi;

/* static */
std::optional<std::unordered_map<std::string, AccessPointAttributes>>
AccessPointAttributes::TryParseJson(const std::string& json)
{
    return ParseAccessPointAttributesFromJson(json);
}

/* static */
std::optional<std::unordered_map<std::string, AccessPointAttributes>>
AccessPointAttributes::TryParseJson(std::istream& json)
{
    return ParseAccessPointAttributesFromJson(json);
}
