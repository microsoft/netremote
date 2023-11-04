
#include <cstdint>
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

        std::cout << std::format("status={} {} {}\n", static_cast<uint32_t>(status.error_code()), status.error_message(), status.error_details());

        REQUIRE(status.ok()); 
        REQUIRE(response.requestid() == request.id());
        REQUIRE(response.payload() == "Pong");
    }
}
