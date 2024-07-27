
#include <string>
#include <unordered_map>

#include <catch2/catch_test_macros.hpp>
#include <microsoft/net/wifi/AccessPointAttributes.hxx>
#include <nlohmann/json.hpp>

TEST_CASE("AccessPointAttributes JSON Serialization and Deserialization", "[wifi][core][ap][serialization]")
{
    using namespace Microsoft::Net::Wifi;

    auto accessPointAttributesJson = R"(
        {
            "wlan0": {
                "Properties": {
                    "key1": "value1",
                    "key2": "value2"
                }
            },
            "wlan1": {
                "Properties": {
                    "key1": "value1",
                    "key2": "value2"
                }
            }
        }
    )";

    // clang-format off
    std::unordered_map<std::string, AccessPointAttributes> AccessPointAttributesMap{
        { 
            "wlan0", 
            AccessPointAttributes{
                {
                    { "key1", "value1" },
                    { "key2", "value2" }
                }
            },
        },
        {
            "wlan1", 
            AccessPointAttributes{
                {
                    { "key1", "value1" },
                    { "key2", "value2" }
                }
            }
        }
    };
    // clang-format on

    SECTION("Deserialization (direct) doesn't cause a crash")
    {
        REQUIRE_NOTHROW(nlohmann::json::parse(accessPointAttributesJson));
    }

    SECTION("Deserialization (wrapped) doesn't cause a crash")
    {
        REQUIRE_NOTHROW(AccessPointAttributes::TryParseJson(accessPointAttributesJson));
    }

    SECTION("Deserialization (wrapped) populates the access point attributes")
    {
        auto deserializedAccessPointAttributesOpt = AccessPointAttributes::TryParseJson(accessPointAttributesJson);
        REQUIRE(deserializedAccessPointAttributesOpt.has_value());
        auto& deserializedAccessPointAttributes = deserializedAccessPointAttributesOpt.value();
        REQUIRE(std::size(deserializedAccessPointAttributes) == std::size(AccessPointAttributesMap));

        for (const auto& [interfaceName, accessPointAttributes] : AccessPointAttributesMap) {
            REQUIRE(deserializedAccessPointAttributes.contains(interfaceName));
            REQUIRE(deserializedAccessPointAttributes[interfaceName].Properties == accessPointAttributes.Properties);
        }
    }
}
