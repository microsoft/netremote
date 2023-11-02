
#include <cstdint>
#include <iostream>

#include <catch2/catch_test_macros.hpp>
#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>
#include <microsoft/wifi/remote/WifiRemoteService.hxx>
#include <WifiRemoteService.grpc.pb.h>

namespace detail
{
constexpr auto RemoteServiceAddressHttp = "http://localhost:5047";
constexpr auto RemoteServiceAddressHttps = "https://localhost:7073";
} // namespace detail

TEST_CASE("wifi remote service can be reached", "[basic][rpc][client]")
{
    using namespace Microsoft::Wifi::Remote;
    using namespace Microsoft::Wifi::Remote::Service;

    SECTION("GetWifiApControl can be called")
    {
        auto channel = grpc::CreateChannel(detail::RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
        auto client = WifiRemote::NewStub(channel);

        Microsoft::Wifi::Remote::Request request{};
        request.set_id("TestWifiRemoteServiceClientTest1");
        request.set_payload("Ping");

        Microsoft::Wifi::Remote::Response response{};
        grpc::ClientContext clientContext{};

        auto status = client->GetWifiApControl(&clientContext, request, &response);

        std::cout << std::format("status={} {} {}\n", static_cast<uint32_t>(status.error_code()), status.error_message(), status.error_details());

        REQUIRE(status.ok()); 
        REQUIRE(response.requestid() == request.id());
        REQUIRE(response.payload() == "Pong");
    }
}
