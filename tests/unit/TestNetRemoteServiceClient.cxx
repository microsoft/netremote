
#include <array>
#include <cstdint>
#include <format>
#include <iostream>

#include <catch2/catch_test_macros.hpp>
#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>
#include <microsoft/net/remote/NetRemoteService.hxx>
#include <NetRemoteService.grpc.pb.h>

namespace detail
{
constexpr auto RemoteServiceAddressHttp = "localhost:5047";
[[maybe_unused]] constexpr auto RemoteServiceAddressHttps = "localhost:7073";

// TODO: use magic_enum instead
static constexpr std::array<Microsoft::Net::Wifi::RadioBand, 3> AllRadioBands = {
    Microsoft::Net::Wifi::RadioBand::RadioBandUnknown,
    Microsoft::Net::Wifi::RadioBand::RadioBand2400MHz,
    Microsoft::Net::Wifi::RadioBand::RadioBand5000MHz,
};

// TODO: use magic_enum instead
static constexpr std::array<Microsoft::Net::Wifi::Dot11PhyType, 9> AllPhyTypes = {
  Microsoft::Net::Wifi::Dot11PhyType::Dot11PhyTypeUnknown,
  Microsoft::Net::Wifi::Dot11PhyType::Dot11PhyTypeB,
  Microsoft::Net::Wifi::Dot11PhyType::Dot11PhyTypeG,
  Microsoft::Net::Wifi::Dot11PhyType::Dot11PhyTypeN,
  Microsoft::Net::Wifi::Dot11PhyType::Dot11PhyTypeA,
  Microsoft::Net::Wifi::Dot11PhyType::Dot11PhyTypeAC,
  Microsoft::Net::Wifi::Dot11PhyType::Dot11PhyTypeAD,
  Microsoft::Net::Wifi::Dot11PhyType::Dot11PhyTypeAX,
  Microsoft::Net::Wifi::Dot11PhyType::Dot11PhyTypeBE,
};
} // namespace detail

TEST_CASE("net remote service can be reached", "[basic][rpc][client]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;

    auto channel = grpc::CreateChannel(detail::RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
    auto client = NetRemote::NewStub(channel);

    SECTION("GetWifiApControl can be called")
    {
        Microsoft::Net::Remote::Request request{};
        request.set_id("TestNetRemoteServiceClientTest1");
        request.set_payload("Ping");

        Microsoft::Net::Remote::Response response{};
        grpc::ClientContext clientContext{};

        auto status = client->GetWifiApControl(&clientContext, request, &response);

        REQUIRE(status.ok()); 
        REQUIRE(response.requestid() == request.id());
        REQUIRE(response.payload() == "Pong");
    }

    SECTION("WifiConfigureAccessPoint can be called")
    {
        for (const auto& band : detail::AllRadioBands)
        {
            for (const auto& phyType : detail::AllPhyTypes)
            {
                Microsoft::Net::Wifi::AccessPointConfiguration apConfiguration{};
                apConfiguration.set_phytype(phyType);

                Microsoft::Net::Remote::Wifi::WifiConfigureAccessPointRequest request{};
                request.set_accesspointid(std::format("TestWifiConfigureAccessPoint"));
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
