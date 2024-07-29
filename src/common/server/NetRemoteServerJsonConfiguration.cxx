
#include <filesystem>
#include <format>
#include <fstream>
#include <optional>

#include <microsoft/net/remote/service/NetRemoteServerJsonConfiguration.hxx>
#include <microsoft/net/wifi/AccessPointAttributesJsonSerialization.hxx>
#include <nlohmann/json.hpp>
#include <plog/Log.h>

using namespace Microsoft::Net::Remote::Service;

/* static */
std::optional<nlohmann::json>
NetRemoteServerJsonConfiguration::ParseFromFile(const std::filesystem::path& configurationFilePath) noexcept
{
    std::ifstream configurationFileStream{ configurationFilePath };
    nlohmann::json configurationJson;

    try {
        configurationJson = nlohmann::json::parse(configurationFileStream);
    } catch (const nlohmann::json::parse_error& parseError) {
        LOGE << std::format("Failed to parse JSON configuration file '{}' ({})", configurationFilePath.string(), parseError.what());
        return std::nullopt;
    }

    LOGD << std::format("Successfully parsed JSON configuration file '{}'\n{}", configurationFilePath.string(), configurationJson.dump(2));

    return configurationJson;
}

/* static */
std::optional<NetRemoteServerJsonConfiguration>
NetRemoteServerJsonConfiguration::TryParseFromJson(const nlohmann::json& configurationJson) noexcept
{
    using Microsoft::Net::Wifi::AccessPointAttributes;

    NetRemoteServerJsonConfiguration configuration{};

    if (configurationJson.contains(AccessPointAttributesKey)) {
        try {
            auto accessPointAttributesJson = configurationJson.at(AccessPointAttributesKey);
            if (!accessPointAttributesJson.is_object()) {
                LOGW << std::format("JSON configuration '{}' is not an object; ignoring", AccessPointAttributesKey);
            }

            std::unordered_map<std::string, AccessPointAttributes> accessPointAttributes{};
            accessPointAttributesJson.get_to(accessPointAttributes);
            configuration.AccessPointAttributes = std::move(accessPointAttributes);
        } catch (const nlohmann::json::exception& jsonException) {
            LOGW << std::format("Failed to parse JSON configuration for '{}' field: {}; ignoring", AccessPointAttributesKey, jsonException.what());
        }
    }

    return configuration;
}
