
#include <optional>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>
#include <grpcpp/create_channel.h>
#include <microsoft/net/remote/NetRemoteServer.hxx>
#include <microsoft/net/remote/protocol/NetRemoteService.grpc.pb.h>

#include "TestNetRemoteCommon.hxx"

TEST_CASE("Create a NetRemoteServer instance", "[basic][rpc][remote]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Wifi;

    NetRemoteServerConfiguration Configuration{
        .ServerAddress = Test::RemoteServiceAddressHttp,
        .AccessPointManager = AccessPointManager::Create(),
    };

    SECTION("Create doesn't cause a crash")
    {
        REQUIRE_NOTHROW(NetRemoteServer{ Configuration });
    }
}

TEST_CASE("Destroy a NetRemoteServer instance", "[basic][rpc][remote]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Wifi;

    NetRemoteServerConfiguration Configuration{
        .ServerAddress = Test::RemoteServiceAddressHttp,
        .AccessPointManager = AccessPointManager::Create(),
    };

    std::optional<NetRemoteServer> server{ Configuration };
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
        const auto clients = Test::EstablishClientConnections(static_cast<std::size_t>(numClientsToCreate.get()));

        REQUIRE_NOTHROW(server.reset());
    }
}

TEST_CASE("NetRemoteServer can be reached", "[basic][rpc][remote]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Wifi;

    NetRemoteServerConfiguration Configuration{
        .ServerAddress = Test::RemoteServiceAddressHttp,
        .AccessPointManager = AccessPointManager::Create(),
    };

    NetRemoteServer server{ Configuration };
    server.Run();

    SECTION("Can be reached using insecure channel")
    {
        auto channel = grpc::CreateChannel(Test::RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
        auto client = NetRemote::NewStub(channel);

        REQUIRE(channel->WaitForConnected(std::chrono::system_clock::now() + Test::RemoteServiceConnectionTimeout));
    }
}

TEST_CASE("NetRemoteServer shuts down correctly", "[basic][rpc][remote]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Wifi;

    NetRemoteServerConfiguration Configuration{
        .ServerAddress = Test::RemoteServiceAddressHttp,
        .AccessPointManager = AccessPointManager::Create(),
    };

    NetRemoteServer server{ Configuration };
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
        const auto clients = Test::EstablishClientConnections(static_cast<std::size_t>(numClientsToCreate.get()));

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
        const auto clients = Test::EstablishClientConnections(static_cast<std::size_t>(numClientsToCreate.get()));

        REQUIRE_NOTHROW(server.Stop());
        REQUIRE(server.GetGrpcServer() == nullptr);
    }
}

TEST_CASE("NetRemoteServer can be cycled through run/stop states", "[basic][rpc][remote]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Wifi;

    NetRemoteServerConfiguration Configuration{
        .ServerAddress = Test::RemoteServiceAddressHttp,
        .AccessPointManager = AccessPointManager::Create(),
    };

    NetRemoteServer server{ Configuration };
    REQUIRE_NOTHROW(server.Run());

    SECTION("Can be cycled multiple times")
    {
        const auto numCycles = Catch::Generators::range(1, 3);

        for ([[maybe_unused]] auto _ : std::views::iota(0, numCycles.get())) {
            REQUIRE_NOTHROW(server.Stop());
            REQUIRE_NOTHROW(server.Run());

            auto channel = grpc::CreateChannel(Test::RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
            auto client = NetRemote::NewStub(channel);
            REQUIRE(channel->WaitForConnected(std::chrono::system_clock::now() + Test::RemoteServiceConnectionTimeout));
        }
    }
}
