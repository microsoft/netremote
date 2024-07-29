
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <ranges>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <grpc/impl/codegen/connectivity_state.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <microsoft/net/remote/protocol/NetRemoteService.grpc.pb.h>
#include <microsoft/net/remote/service/NetRemoteServer.hxx>
#include <microsoft/net/remote/service/NetRemoteServerConfiguration.hxx>
#include <microsoft/net/remote/service/NetRemoteServerJsonConfiguration.hxx>
#include <microsoft/net/wifi/AccessPointManager.hxx>

#include "TestNetRemoteCommon.hxx"

using namespace Microsoft::Net::Remote::Test;

namespace Microsoft::Net::Remote::Service::Test
{
/**
 * @brief Create a valid JSON server configuration file.
 *
 * @return std::filesystem::path The path to the created JSON configuration file.
 */
std::filesystem::path
CreateServerConfigurationJsonFile()
{
    const auto configurationFilePath{ std::filesystem::temp_directory_path() / "NetRemoteServerConfiguration.json" };
    std::ofstream configurationFileStream{ configurationFilePath, std::ios::out | std::ios::trunc };

    configurationFileStream << R"({
    "WifiAccessPointAttributes": {
        "wlan0": {
            "Properties": {
                "key0": "value0",
                "key1": "value1",
                "RfAttenutatorChannels": "1,2,3,4,5"
            }
        },
        "wlan1": {
            "Properties": {
                "key0": "value0",
                "key1": "value1",
                "key2": "value2",
                "RfAttenutatorChannels": "6,7,8"
            }
        }
    }
    })";

    return configurationFilePath;
}

/**
 * @brief Get the NetRemoteServerJsonConfiguration object corresponding to the JSON created by
 * CreateServerConfigurationJsonFile().
 *
 * @return const NetRemoteServerJsonConfiguration&
 */
const NetRemoteServerJsonConfiguration&
GetNetRemoteServerJsonConfigurationValidObject()
{
    using Microsoft::Net::Wifi::AccessPointAttributes;

    static const NetRemoteServerJsonConfiguration netRemoteServerJsonConfiguration{
        .AccessPointAttributes = std::unordered_map<std::string, AccessPointAttributes>{
            { "wlan0", { .Properties = { { "key0", "value0" }, { "key1", "value1" }, { "RfAttenutatorChannels", "1,2,3,4,5" } } } },
            { "wlan1", { .Properties = { { "key0", "value0" }, { "key1", "value1" }, { "key2", "value2" }, { "RfAttenutatorChannels", "6,7,8" } } } },
        }
    };

    return netRemoteServerJsonConfiguration;
}

/**
 * @brief Create an invalid JSON server configuration file.
 *
 * @return std::filesystem::path The path to the created JSON configuration file.
 */
std::filesystem::path
CreateServerConfigurationJsonFileInvalid()
{
    const auto configurationFilePath{ std::filesystem::temp_directory_path() / "NetRemoteServerConfigurationInvalid.json" };
    std::ofstream configurationFileStream{ configurationFilePath, std::ios::out | std::ios::trunc };

    configurationFileStream << "This is a bunch of non-JSON content";

    return configurationFilePath;
}
} // namespace Microsoft::Net::Remote::Service::Test

TEST_CASE("Parse JSON server configuration from file", "[basic][json][config]")
{
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Wifi;

    const auto ConfigurationFileJsonValidPath{ Test::CreateServerConfigurationJsonFile() };
    const auto ConfigurationFileJsonInvalidPath{ Test::CreateServerConfigurationJsonFileInvalid() };

    SECTION("ParseFromFile doesn't cause a crash with valid JSON file")
    {
        REQUIRE_NOTHROW(NetRemoteServerJsonConfiguration::ParseFromFile(ConfigurationFileJsonValidPath));
    }

    SECTION("ParseFromFile doesn't cause a crash with invalid JSON file")
    {
        REQUIRE_NOTHROW(NetRemoteServerJsonConfiguration::ParseFromFile(ConfigurationFileJsonInvalidPath));
    }

    SECTION("TryParseFromJson doesn't cause a crash with valid JSON configuration content")
    {
        REQUIRE_NOTHROW(NetRemoteServerJsonConfiguration::TryParseFromJson(NetRemoteServerJsonConfiguration::ParseFromFile(ConfigurationFileJsonValidPath).value()));
    }

    SECTION("TryParseFromJson doesn't cause a crash with empty JSON configuration content")
    {
        REQUIRE_NOTHROW(NetRemoteServerJsonConfiguration::TryParseFromJson(nlohmann::json{}));
    }

    SECTION("TryParseFromJson parses WifiAccessPointAttributes from valid JSON configuration content")
    {
        const auto configurationJson{ NetRemoteServerJsonConfiguration::ParseFromFile(ConfigurationFileJsonValidPath).value() };
        const auto configuration{ NetRemoteServerJsonConfiguration::TryParseFromJson(configurationJson).value() };

        // Ensure parsing succeeded.
        REQUIRE(configuration.AccessPointAttributes.has_value());

        // Ensure Wi-Fi access point properties were parsed.
        const auto& accessPointAttributes = configuration.AccessPointAttributes.value();
        REQUIRE(std::size(accessPointAttributes) == 2);
        REQUIRE(accessPointAttributes.contains("wlan0"));
        REQUIRE(accessPointAttributes.contains("wlan1"));

        // Validate the properties of each access point encoded in the JSON.

        // Validate the wlan0 access point properties.
        const auto& wlan0Attributes = configuration.AccessPointAttributes->at("wlan0");
        const auto& wlan0Properties = wlan0Attributes.Properties;
        REQUIRE(std::size(wlan0Properties) == 3);
        REQUIRE(wlan0Properties.contains("key0"));
        REQUIRE(wlan0Properties.at("key0") == "value0");
        REQUIRE(wlan0Properties.contains("key1"));
        REQUIRE(wlan0Properties.at("key1") == "value1");
        REQUIRE(wlan0Properties.contains("RfAttenutatorChannels"));
        REQUIRE(wlan0Properties.at("RfAttenutatorChannels") == "1,2,3,4,5");

        // Validate the wlan1 access point properties.
        const auto& wlan1Attributes = configuration.AccessPointAttributes->at("wlan1");
        const auto& wlan1Properties = wlan1Attributes.Properties;
        REQUIRE(std::size(wlan1Properties) == 4);
        REQUIRE(wlan1Properties.contains("key0"));
        REQUIRE(wlan1Properties.at("key0") == "value0");
        REQUIRE(wlan1Properties.contains("key1"));
        REQUIRE(wlan1Properties.at("key1") == "value1");
        REQUIRE(wlan1Properties.contains("key2"));
        REQUIRE(wlan1Properties.at("key2") == "value2");
        REQUIRE(wlan1Properties.contains("RfAttenutatorChannels"));
        REQUIRE(wlan1Properties.at("RfAttenutatorChannels") == "6,7,8");

        // Validate the parsed object in totality.
        const auto& netRemoteServerJsonConfigurationValid = Test::GetNetRemoteServerJsonConfigurationValidObject();
        REQUIRE(netRemoteServerJsonConfigurationValid == configuration);
    }
}

TEST_CASE("Create a NetRemoteServer instance", "[basic][rpc][remote]")
{
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Wifi;

    const auto serverConfiguration = CreateServerConfiguration();

    SECTION("Create doesn't cause a crash")
    {
        REQUIRE_NOTHROW(NetRemoteServer{ serverConfiguration });
    }
}

TEST_CASE("Destroy a NetRemoteServer instance", "[basic][rpc][remote]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Wifi;

    const auto serverConfiguration = CreateServerConfiguration();
    std::optional<NetRemoteServer> server{ serverConfiguration };
    server->Run();

    SECTION("Destroy doesn't cause a crash")
    {
        REQUIRE_NOTHROW(server.reset());
    }

    SECTION("Destroy doesn't cause a crash with connected clients")
    {
        // Vary the number of connected clients and validate test section for each.
        const auto numClientsToCreate = Catch::Generators::range(1, 2);

        // Establish client connections to the server.
        const auto clients = EstablishClientConnections(static_cast<std::size_t>(numClientsToCreate.get()));

        REQUIRE_NOTHROW(server.reset());
    }
}

TEST_CASE("NetRemoteServer can be reached", "[basic][rpc][remote]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Wifi;

    const auto serverConfiguration = CreateServerConfiguration();
    NetRemoteServer server{ serverConfiguration };
    server.Run();

    SECTION("Can be reached using insecure channel")
    {
        auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
        auto client = NetRemote::NewStub(channel);

        REQUIRE(channel->WaitForConnected(std::chrono::system_clock::now() + RemoteServiceConnectionTimeout));
    }
}

TEST_CASE("NetRemoteServer shuts down correctly", "[basic][rpc][remote]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Wifi;

    const auto serverConfiguration = CreateServerConfiguration();
    NetRemoteServer server{ serverConfiguration };
    server.Run();

    SECTION("Stop() doesn't cause a crash with no connected clients")
    {
        REQUIRE_NOTHROW(server.Stop());
    }

    SECTION("Stop() severs connections for connected clients")
    {
        // Vary the number of connected clients and validate test section for each.
        const auto numClientsToCreate = Catch::Generators::range(1, 3);

        // Establish client connections to the server.
        const auto clients = EstablishClientConnections(static_cast<std::size_t>(numClientsToCreate.get()));

        // Stop the server.
        REQUIRE_NOTHROW(server.Stop());

        // Validate each channel is in IDLE state.
        for (const auto& [channel, _] : clients) {
            REQUIRE(channel->GetState(false) == GRPC_CHANNEL_IDLE);
        }
    }

    SECTION("Stop() invalidates gRPC server instance with no connected clients")
    {
        REQUIRE_NOTHROW(server.Stop());
        REQUIRE(server.GetGrpcServer() == nullptr);
    }

    SECTION("Stop() invalidates gRPC server instance with connected clients")
    {
        // Vary the number of connected clients and validate test section for each.
        const auto numClientsToCreate = Catch::Generators::range(1, 3);

        // Establish client connections to the server.
        const auto clients = EstablishClientConnections(static_cast<std::size_t>(numClientsToCreate.get()));

        REQUIRE_NOTHROW(server.Stop());
        REQUIRE(server.GetGrpcServer() == nullptr);
    }
}

TEST_CASE("NetRemoteServer can be cycled through run/stop states", "[basic][rpc][remote]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Wifi;

    const auto serverConfiguration = CreateServerConfiguration();
    NetRemoteServer server{ serverConfiguration };
    REQUIRE_NOTHROW(server.Run());

    SECTION("Can be cycled multiple times")
    {
        const auto numCycles = Catch::Generators::range(1, 3);

        for ([[maybe_unused]] auto _ : std::views::iota(0, numCycles.get())) {
            REQUIRE_NOTHROW(server.Stop());
            REQUIRE_NOTHROW(server.Run());

            auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
            auto client = NetRemote::NewStub(channel);
            REQUIRE(channel->WaitForConnected(std::chrono::system_clock::now() + RemoteServiceConnectionTimeout));
        }
    }
}
