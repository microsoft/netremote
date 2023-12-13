
#include <format>
#include <iostream>

#include <catch2/catch_test_macros.hpp>
#include <grpcpp/create_channel.h>
#include <magic_enum.hpp>
#include <microsoft/net/remote/NetRemoteServer.hxx>
#include <microsoft/net/remote/protocol/NetRemoteService.grpc.pb.h>

#include "TestNetRemoteCommon.hxx"

TEST_CASE("WifiConfigureAccessPoint API", "[basic][rpc][client][remote]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Remote::Wifi;

    NetRemoteServer server{ Test::RemoteServiceAddressHttp };
    server.Run();

    auto channel = grpc::CreateChannel(Test::RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
    auto client = NetRemote::NewStub(channel);

    SECTION("Can be called")
    {
        for (const auto& band : magic_enum::enum_values<Microsoft::Net::Wifi::RadioBand>())
        {
            for (const auto& phyType : magic_enum::enum_values<Microsoft::Net::Wifi::Dot11PhyType>())
            {
                Microsoft::Net::Wifi::AccessPointConfiguration apConfiguration{};
                apConfiguration.set_phytype(phyType);

                WifiConfigureAccessPointRequest request{};
                request.set_accesspointid(std::format("TestWifiConfigureAccessPoint{}", magic_enum::enum_name(band)));
                request.set_defaultband(band);
                request.mutable_configurations()->emplace(band, apConfiguration);

                WifiConfigureAccessPointResult result{};
                grpc::ClientContext clientContext{};

                auto status = client->WifiConfigureAccessPoint(&clientContext, request, &result);

                REQUIRE(status.ok());
                REQUIRE(result.succeeded() == true);
                REQUIRE(result.accesspointid() == request.accesspointid());
            }
        }
    }
}

TEST_CASE("WifiEnumerateAccessPoints API", "[basic][rpc][client][remote]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Remote::Wifi;

    NetRemoteServer server{ Test::RemoteServiceAddressHttp };
    server.Run();

    auto channel = grpc::CreateChannel(Test::RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
    auto client = NetRemote::NewStub(channel);

    SECTION("Can be called")
    {
        WifiEnumerateAccessPointsRequest request{};

        WifiEnumerateAccessPointsResult result{};
        grpc::ClientContext clientContext{};

        auto status = client->WifiEnumerateAccessPoints(&clientContext, request, &result);
        REQUIRE(status.ok());
        REQUIRE_NOTHROW([&]{ [[maybe_unused]] const auto& accessPoints = result.accesspoints(); });
    }

    SECTION("Initial enablement status is disabled")
    {
        WifiEnumerateAccessPointsRequest request{};

        WifiEnumerateAccessPointsResult result{};
        grpc::ClientContext clientContext{};

        auto status = client->WifiEnumerateAccessPoints(&clientContext, request, &result);
        REQUIRE(status.ok());

        for (const auto& accessPoint : result.accesspoints())
        {
            REQUIRE(!accessPoint.isenabled());
        }
    }
}
