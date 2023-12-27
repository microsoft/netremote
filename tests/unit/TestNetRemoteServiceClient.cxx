
#include <format>
#include <iostream>
#include <string>
#include <string_view>

#include <catch2/catch_test_macros.hpp>
#include <grpcpp/create_channel.h>
#include <magic_enum.hpp>
#include <microsoft/net/remote/NetRemoteServer.hxx>
#include <microsoft/net/remote/protocol/NetRemoteService.grpc.pb.h>

#include "TestNetRemoteCommon.hxx"

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
        REQUIRE_NOTHROW([&] {
            [[maybe_unused]] const auto& accessPoints = result.accesspoints();
        });
    }

    SECTION("Initial enablement status is disabled")
    {
        WifiEnumerateAccessPointsRequest request{};

        WifiEnumerateAccessPointsResult result{};
        grpc::ClientContext clientContext{};

        auto status = client->WifiEnumerateAccessPoints(&clientContext, request, &result);
        REQUIRE(status.ok());

        for (const auto& accessPoint : result.accesspoints()) {
            REQUIRE(!accessPoint.isenabled());
        }
    }
}

TEST_CASE("WifiAccessPointEnable API", "[basic][rpc][client][remote]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Remote::Wifi;
    using namespace Microsoft::Net::Wifi;

    constexpr auto SsidName{ "TestWifiAccessPointEnable" };

    NetRemoteServer server{ Test::RemoteServiceAddressHttp };
    server.Run();

    auto channel = grpc::CreateChannel(Test::RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
    auto client = NetRemote::NewStub(channel);

    SECTION("Can be called")
    {
        AccessPointConfiguration apConfiguration{};
        apConfiguration.mutable_ssid()->set_name(SsidName);
        apConfiguration.set_phytype(Dot11PhyType::Dot11PhyTypeA);
        apConfiguration.set_authenticationalgorithm(Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmSharedKey);
        apConfiguration.set_encryptionalgorithm(Dot11CipherAlgorithm::Dot11CipherAlgorithmCcmp256);
        apConfiguration.mutable_bands()->Add(RadioBand::RadioBand2_4GHz);
        apConfiguration.mutable_bands()->Add(RadioBand::RadioBand5_0GHz);

        WifiAccessPointEnableRequest request{};
        request.set_accesspointid("TestWifiAccessPointEnable");
        *request.mutable_configuration() = std::move(apConfiguration);

        WifiAccessPointEnableResult result{};
        grpc::ClientContext clientContext{};

        auto status = client->WifiAccessPointEnable(&clientContext, request, &result);
        REQUIRE(status.ok());
        REQUIRE(result.accesspointid() == request.accesspointid());
        REQUIRE(result.has_status());
        REQUIRE(result.status().code() == WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);
        REQUIRE(result.status().message().empty());
        REQUIRE(result.status().has_details() == false);
    }
}
