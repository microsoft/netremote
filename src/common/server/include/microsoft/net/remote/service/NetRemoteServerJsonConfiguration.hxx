
#ifndef NET_REMOTE_SERVER_JSON_CONFIGURATION_HXX
#define NET_REMOTE_SERVER_JSON_CONFIGURATION_HXX

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>

#include <microsoft/net/wifi/AccessPointAttributes.hxx>
#include <nlohmann/json.hpp>

namespace Microsoft::Net::Remote::Service
{
/**
 * @brief Helper class to parse a JSON configuration file for the NetRemoteServer.
 */
struct NetRemoteServerJsonConfiguration
{
    std::optional<std::unordered_map<std::string, Microsoft::Net::Wifi::AccessPointAttributes>> AccessPointAttributes{};

    bool
    operator==(const NetRemoteServerJsonConfiguration&) const = default;

    /**
     * @brief Parse a JSON configuration file.
     *
     * @param configurationFilePath
     * @return std::optional<nlohmann::json>
     */
    static std::optional<nlohmann::json>
    ParseFromFile(const std::filesystem::path& configurationFilePath) noexcept;

    /**
     * @brief Try to parse a NetRemoteServerJsonConfiguration object from a JSON object.
     *
     * @param configurationJson The JSON object to parse.
     * @return std::optional<NetRemoteServerJsonConfiguration>
     */
    static std::optional<NetRemoteServerJsonConfiguration>
    TryParseFromJson(const nlohmann::json& configurationJson) noexcept;

protected:
    static constexpr auto AccessPointAttributesKey = "WifiAccessPointAttributes";
};
} // namespace Microsoft::Net::Remote::Service

#endif // NET_REMOTE_SERVER_JSON_CONFIGURATION_HXX
