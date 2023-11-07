
#include <array>
#include <cstdint>
#include <format>
#include <iostream>

#include <catch2/catch_test_macros.hpp>
#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>
#include <magic_enum.hpp>
#include <microsoft/net/remote/NetRemoteService.hxx>
#include <NetRemoteService.grpc.pb.h>

namespace detail
{
constexpr auto RemoteServiceAddressHttp = "localhost:5047";
[[maybe_unused]] constexpr auto RemoteServiceAddressHttps = "localhost:7073";
} // namespace detail

TEST_CASE("net remote service can be reached", "[basic][rpc][client]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;

    auto channel = grpc::CreateChannel(detail::RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
    auto client = NetRemote::NewStub(channel);

    SECTION("WifiConfigureAccessPoint can be called")
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
