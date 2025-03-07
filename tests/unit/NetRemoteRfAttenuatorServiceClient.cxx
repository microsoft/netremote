#include <plog/Log.h>

#include <catch2/catch_test_macros.hpp>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/impl/codegen/status.h>
#include <grpcpp/security/credentials.h>

#include <microsoft/net/remote/protocol/NetRemoteRfAttenuator.grpc.pb.h>
#include <microsoft/net/remote/protocol/NetRemoteRfAttenuatorService.grpc.pb.h>
#include <microsoft/net/remote/service/NetRemoteServer.hxx>

#include "TestNetRemoteCommon.hxx"

using namespace Microsoft::Net::Remote::Test;
using namespace Microsoft::Net::Remote::Service;

TEST_CASE("RfAttenuator IsEnabled API", "[basic][rpc][client][remote][rfAttenuator]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Wifi;
    using namespace Microsoft::Net::Wifi;

    const auto serverConfiguration = CreateServerConfiguration();
    NetRemoteServer server{ serverConfiguration };
    server.Run();

    auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
    auto client = NetRemoteRfAttenuator::NewStub(channel);

    LOGI << "gRPC initialized";

    SECTION("IsEnabled is true")
    {
        const google::protobuf::Empty request{};
        google::protobuf::BoolValue result{};
        grpc::ClientContext clientContext{};

        auto status = client->IsEnabled(&clientContext, request, &result);
        REQUIRE(status.ok());
        REQUIRE(result.value());
    }

    SECTION("IsEnabled is false")
    {
        const google::protobuf::Empty request{};
        google::protobuf::BoolValue result{};
        grpc::ClientContext clientContext{};

        auto status = client->IsEnabled(&clientContext, request, &result);
        REQUIRE(status.ok());
        REQUIRE(!result.value());
    }
}