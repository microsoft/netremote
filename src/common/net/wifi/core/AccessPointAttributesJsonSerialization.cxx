
#include <microsoft/net/wifi/AccessPointAttributes.hxx>
#include <microsoft/net/wifi/AccessPointAttributesJsonSerialization.hxx>
#include <nlohmann/json.hpp>

namespace Microsoft::Net::Wifi
{
void
to_json(nlohmann::json& accessPointAttributesJson, const AccessPointAttributes& accessPointAttributes)
{
    accessPointAttributesJson = nlohmann::json{
        { "Properties", accessPointAttributes.Properties }
    };
}

void
from_json(const nlohmann::json& accessPointAttributesJson, AccessPointAttributes& accessPointAttributes)
{
    accessPointAttributesJson.at("Properties").get_to(accessPointAttributes.Properties);
}

} // namespace Microsoft::Net::Wifi
