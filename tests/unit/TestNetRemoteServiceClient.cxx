
#include <format>
#include <iostream>

#include <catch2/catch_test_macros.hpp>
#include <grpcpp/create_channel.h>
#include <magic_enum.hpp>
#include <microsoft/net/remote/NetRemoteServer.hxx>
#include <microsoft/net/remote/protocol/NetRemoteService.grpc.pb.h>

#include "TestNetRemoteCommon.hxx"

TEST_CASE("WifiConfigureAccessPoint API can be called", "[basic][rpc][client][remote]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;

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

                Microsoft::Net::Remote::Wifi::WifiConfigureAccessPointRequest request{};
                request.set_accesspointid(std::format("TestWifiConfigureAccessPoint{}", magic_enum::enum_name(band)));
                request.set_defaultband(band);
                request.mutable_configurations()->emplace(band, apConfiguration);

                Microsoft::Net::Remote::Wifi::WifiConfigureAccessPointResult result{};
                grpc::ClientContext clientContext{};

                auto status = client->WifiConfigureAccessPoint(&clientContext, request, &result);

                REQUIRE(status.ok());
                REQUIRE(result.succeeded() == true);
                REQUIRE(result.accesspointid() == request.accesspointid());
            }
        }
    }
}
