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
using namespace Microsoft::Net::Remote;
using namespace Microsoft::Net::Remote::RfAttenuator;

TEST_CASE("RfAttenuator IsEnabled API", "[basic][rpc][client][remote][rfAttenuator]")
{
    SECTION("IsEnabled is true")
    {
        auto serverConfiguration = CreateServerConfiguration();
        serverConfiguration.RfAttenuatorConfiguration.Type = RfAttenuatorType::Software;
        NetRemoteServer server{ serverConfiguration };
        server.Run();

        auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
        auto client = NetRemoteRfAttenuator::NewStub(channel);

        LOGI << "gRPC initialized";

        const google::protobuf::Empty request{};
        google::protobuf::BoolValue result{};
        grpc::ClientContext clientContext{};

        auto status = client->IsEnabled(&clientContext, request, &result);
        REQUIRE(status.ok());
        REQUIRE(result.value());
    }

    SECTION("IsEnabled is false")
    {
        // serverConfiguration.RfAttenuatorConfiguration.Type is RfAttenuatorType::None by default
        auto serverConfiguration = CreateServerConfiguration();
        NetRemoteServer server{ serverConfiguration };
        server.Run();

        auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
        auto client = NetRemoteRfAttenuator::NewStub(channel);

        LOGI << "gRPC initialized";

        const google::protobuf::Empty request{};
        google::protobuf::BoolValue result{};
        grpc::ClientContext clientContext{};

        auto status = client->IsEnabled(&clientContext, request, &result);
        REQUIRE(!status.ok());
        REQUIRE(status.error_code() == grpc::StatusCode::UNIMPLEMENTED);
        REQUIRE(!result.value());
    }
}

TEST_CASE("RfAttenuator Reset API", "[basic][rpc][client][remote][rfAttenuator]")
{
    SECTION("Reset succeeds when RfAttenuatorType is Software")
    {
        auto serverConfiguration = CreateServerConfiguration();
        serverConfiguration.RfAttenuatorConfiguration.Type = RfAttenuatorType::Software;
        NetRemoteServer server{ serverConfiguration };
        server.Run();

        auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
        auto client = NetRemoteRfAttenuator::NewStub(channel);

        LOGI << "gRPC initialized";

        const google::protobuf::Empty request{};
        ResetResult result{};
        grpc::ClientContext clientContext{};

        auto status = client->Reset(&clientContext, request, &result);
        REQUIRE(status.ok());
        REQUIRE(result.status().code() == RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeSucceeded);
    }

    SECTION("Reset fails when RfAttenuatorType is None")
    {
        // serverConfiguration.RfAttenuatorConfiguration.Type is RfAttenuatorType::None by default
        auto serverConfiguration = CreateServerConfiguration();
        NetRemoteServer server{ serverConfiguration };
        server.Run();

        auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
        auto client = NetRemoteRfAttenuator::NewStub(channel);

        LOGI << "gRPC initialized";

        const google::protobuf::Empty request{};
        ResetResult result{};
        grpc::ClientContext clientContext{};

        auto status = client->Reset(&clientContext, request, &result);
        REQUIRE(!status.ok());
        REQUIRE(status.error_code() == grpc::StatusCode::UNIMPLEMENTED);
        REQUIRE(result.status().code() == RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeUnknown);
    }
}

TEST_CASE("RfAttenuator GetProperties API", "[basic][rpc][client][remote][rfAttenuator]")
{
    SECTION("GetProperties succeeds when RfAttenuatorType is Software")
    {
        auto serverConfiguration = CreateServerConfiguration();
        serverConfiguration.RfAttenuatorConfiguration.Type = RfAttenuatorType::Software;
        NetRemoteServer server{ serverConfiguration };
        server.Run();

        auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
        auto client = NetRemoteRfAttenuator::NewStub(channel);

        LOGI << "gRPC initialized";

        const google::protobuf::Empty request{};
        GetPropertiesResult result{};
        grpc::ClientContext clientContext{};

        auto status = client->GetProperties(&clientContext, request, &result);
        REQUIRE(status.ok());
        REQUIRE(result.status().code() == RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeSucceeded);
        REQUIRE(!result.identification().empty());
    }

    SECTION("GetProperties fails when RfAttenuatorType is None")
    {
        // serverConfiguration.RfAttenuatorConfiguration.Type is RfAttenuatorType::None by default
        auto serverConfiguration = CreateServerConfiguration();
        NetRemoteServer server{ serverConfiguration };
        server.Run();

        auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
        auto client = NetRemoteRfAttenuator::NewStub(channel);

        LOGI << "gRPC initialized";

        const google::protobuf::Empty request{};
        GetPropertiesResult result{};
        grpc::ClientContext clientContext{};

        auto status = client->GetProperties(&clientContext, request, &result);
        REQUIRE(!status.ok());
        REQUIRE(status.error_code() == grpc::StatusCode::UNIMPLEMENTED);
        REQUIRE(result.status().code() == RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeUnknown);
    }
}

TEST_CASE("RfAttenuator GetAttenuationForChannel and SetAttenuationForChannel API", "[basic][rpc][client][remote][rfAttenuator]")
{
    SECTION("Get and Set attenuation succeeds when RfAttenuatorType is Software")
    {
        auto serverConfiguration = CreateServerConfiguration();
        serverConfiguration.RfAttenuatorConfiguration.Type = RfAttenuatorType::Software;
        NetRemoteServer server{ serverConfiguration };
        server.Run();

        auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
        auto client = NetRemoteRfAttenuator::NewStub(channel);

        LOGI << "gRPC initialized";

        uint32_t attenuatorChannel = 1;
        auto attenuationDbm = 10.0;
        SetAttenuationRequest setAttenuationRequest{};
        setAttenuationRequest.set_channel(attenuatorChannel);
        setAttenuationRequest.set_attenuationdbm(attenuationDbm);
        SetAttenuationResult setAttenuationResult{};
        grpc::ClientContext setClientContext{};

        GetAttenuationRequest getAttenuationRequest{};
        getAttenuationRequest.set_channel(attenuatorChannel);
        GetAttenuationResult getAttenuationResult{};
        grpc::ClientContext getClientContext{};

        auto status = client->SetAttenuationForChannel(&setClientContext, setAttenuationRequest, &setAttenuationResult);
        REQUIRE(status.ok());
        REQUIRE(setAttenuationResult.status().code() == RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeSucceeded);

        status = client->GetAttenuationForChannel(&getClientContext, getAttenuationRequest, &getAttenuationResult);
        REQUIRE(status.ok());
        REQUIRE(getAttenuationResult.status().code() == RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeSucceeded);
        REQUIRE(getAttenuationResult.attenuationdbm() == attenuationDbm);
    }

    SECTION("Get and Set attenuation fails when RfAttenuatorType is None")
    {
        // serverConfiguration.RfAttenuatorConfiguration.Type is RfAttenuatorType::None by default
        auto serverConfiguration = CreateServerConfiguration();
        NetRemoteServer server{ serverConfiguration };
        server.Run();

        auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
        auto client = NetRemoteRfAttenuator::NewStub(channel);

        LOGI << "gRPC initialized";

        uint32_t attenuatorChannel = 1;
        auto attenuationDbm = 10.0;
        SetAttenuationRequest setAttenuationRequest{};
        setAttenuationRequest.set_channel(attenuatorChannel);
        setAttenuationRequest.set_attenuationdbm(attenuationDbm);
        SetAttenuationResult setAttenuationResult{};
        grpc::ClientContext setClientContext{};

        GetAttenuationRequest getAttenuationRequest{};
        getAttenuationRequest.set_channel(attenuatorChannel);
        GetAttenuationResult getAttenuationResult{};
        grpc::ClientContext getClientContext{};

        auto status = client->SetAttenuationForChannel(&setClientContext, setAttenuationRequest, &setAttenuationResult);
        REQUIRE(!status.ok());
        REQUIRE(status.error_code() == grpc::StatusCode::UNIMPLEMENTED);
        REQUIRE(setAttenuationResult.status().code() == RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeUnknown);

        status = client->GetAttenuationForChannel(&getClientContext, getAttenuationRequest, &getAttenuationResult);
        REQUIRE(!status.ok());
        REQUIRE(status.error_code() == grpc::StatusCode::UNIMPLEMENTED);
        REQUIRE(getAttenuationResult.status().code() == RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeUnknown);
    }

    SECTION("Get and Set attenuation fails when attenuation channel is wrong")
    {
        auto serverConfiguration = CreateServerConfiguration();
        serverConfiguration.RfAttenuatorConfiguration.Type = RfAttenuatorType::Software;
        NetRemoteServer server{ serverConfiguration };
        server.Run();

        auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
        auto client = NetRemoteRfAttenuator::NewStub(channel);

        LOGI << "gRPC initialized";

        uint32_t attenuatorChannel = UINT32_MAX;
        auto attenuationDbm = 10.0;
        SetAttenuationRequest setAttenuationRequest{};
        setAttenuationRequest.set_channel(attenuatorChannel);
        setAttenuationRequest.set_attenuationdbm(attenuationDbm);
        SetAttenuationResult setAttenuationResult{};
        grpc::ClientContext setClientContext{};

        GetAttenuationRequest getAttenuationRequest{};
        getAttenuationRequest.set_channel(attenuatorChannel);
        GetAttenuationResult getAttenuationResult{};
        grpc::ClientContext getClientContext{};

        auto status = client->SetAttenuationForChannel(&setClientContext, setAttenuationRequest, &setAttenuationResult);
        REQUIRE(status.ok());
        REQUIRE(setAttenuationResult.status().code() == RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeFailed);

        status = client->GetAttenuationForChannel(&getClientContext, getAttenuationRequest, &getAttenuationResult);
        REQUIRE(status.ok());
        REQUIRE(getAttenuationResult.status().code() == RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeFailed);
    }

    SECTION("Get and Set attenuation fails when attenuation dbm is wrong")
    {
        auto serverConfiguration = CreateServerConfiguration();
        serverConfiguration.RfAttenuatorConfiguration.Type = RfAttenuatorType::Software;
        NetRemoteServer server{ serverConfiguration };
        server.Run();

        auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
        auto client = NetRemoteRfAttenuator::NewStub(channel);

        LOGI << "gRPC initialized";

        uint32_t attenuatorChannel = 1;
        auto attenuationDbm = UINT32_MAX;
        SetAttenuationRequest setAttenuationRequest{};
        setAttenuationRequest.set_channel(attenuatorChannel);
        setAttenuationRequest.set_attenuationdbm(attenuationDbm);
        SetAttenuationResult setAttenuationResult{};
        grpc::ClientContext setClientContext{};

        GetAttenuationRequest getAttenuationRequest{};
        getAttenuationRequest.set_channel(attenuatorChannel);
        GetAttenuationResult getAttenuationResult{};
        grpc::ClientContext getClientContext{};

        auto status = client->SetAttenuationForChannel(&setClientContext, setAttenuationRequest, &setAttenuationResult);
        REQUIRE(status.ok());
        REQUIRE(setAttenuationResult.status().code() == RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeFailed);

        status = client->GetAttenuationForChannel(&getClientContext, getAttenuationRequest, &getAttenuationResult);
        REQUIRE(status.ok());
        REQUIRE(getAttenuationResult.status().code() == RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeSucceeded);
        // SetAttenuationForChannel failed, so attenuationdbm should be the default 0.0
        REQUIRE(getAttenuationResult.attenuationdbm() == 0.0);
    }
}