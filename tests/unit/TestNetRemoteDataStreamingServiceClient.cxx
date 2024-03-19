
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <thread>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <grpcpp/create_channel.h>
#include <grpcpp/impl/codegen/status.h>
#include <grpcpp/impl/codegen/status_code_enum.h>
#include <grpcpp/security/credentials.h>
#include <microsoft/net/remote/NetRemoteServer.hxx>
#include <microsoft/net/remote/NetRemoteServerConfiguration.hxx>
#include <microsoft/net/remote/protocol/NetRemoteDataStream.pb.h>
#include <microsoft/net/remote/protocol/NetRemoteDataStreamingService.grpc.pb.h>

#include "TestNetRemoteCommon.hxx"
#include "TestNetRemoteDataStreamingReactors.hxx"

TEST_CASE("DataStreamUpload API", "[basic][rpc][client][remote][stream]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::DataStream;
    using namespace Microsoft::Net::Remote::Service;

    using Microsoft::Net::Remote::Test::DataStreamWriter;
    using Microsoft::Net::Remote::Test::RemoteServiceAddressHttp;

    const NetRemoteServerConfiguration Configuration{
        .ServerAddress = RemoteServiceAddressHttp,
    };

    NetRemoteServer server{ Configuration };
    server.Run();

    auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
    auto client = NetRemoteDataStreaming::NewStub(channel);

    static constexpr auto numberOfDataBlocksToWrite = 10;

    SECTION("Can be called with one client")
    {
        auto dataStreamWriter = std::make_unique<DataStreamWriter>(client.get(), numberOfDataBlocksToWrite);

        DataStreamUploadResult result{};
        const grpc::Status status = dataStreamWriter->Await(&result);
        REQUIRE(status.ok());
        REQUIRE(result.numberofdatablocksreceived() == numberOfDataBlocksToWrite);
        REQUIRE(result.status().code() == DataStreamOperationStatusCodeSucceeded);
    }

    SECTION("Can be called with multiple parallel clients")
    {
        static constexpr auto numberOfClients = 5;

        std::vector<std::jthread> clientThreads;
        for (std::size_t i = 0; i < numberOfClients; i++) {
            clientThreads.emplace_back([&]() {
                auto dataStreamingClient = NetRemoteDataStreaming::NewStub(channel);
                auto dataStreamWriter = std::make_unique<DataStreamWriter>(dataStreamingClient.get(), numberOfDataBlocksToWrite);

                DataStreamUploadResult result{};
                const grpc::Status status = dataStreamWriter->Await(&result);
                REQUIRE(status.ok());
                REQUIRE(result.numberofdatablocksreceived() == numberOfDataBlocksToWrite);
                REQUIRE(result.status().code() == DataStreamOperationStatusCodeSucceeded);
            });
        }

        for (auto& clientThread : clientThreads) {
            REQUIRE_NOTHROW(clientThread.join());
        }
    }
}

TEST_CASE("DataStreamDownload API", "[basic][rpc][client][remote][stream]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::DataStream;
    using namespace Microsoft::Net::Remote::Service;

    using Microsoft::Net::Remote::Test::DataStreamReader;
    using Microsoft::Net::Remote::Test::RemoteServiceAddressHttp;

    const NetRemoteServerConfiguration Configuration{
        .ServerAddress = RemoteServiceAddressHttp,
    };

    NetRemoteServer server{ Configuration };
    server.Run();

    auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
    auto client = NetRemoteDataStreaming::NewStub(channel);

    static constexpr auto fixedNumberOfDataBlocksToStream = 10;

    SECTION("Can be called with DataStreamTypeFixed and DataStreamPatternConstant")
    {
        DataStreamFixedTypeProperties fixedTypeProperties{};
        fixedTypeProperties.set_numberofdatablockstostream(fixedNumberOfDataBlocksToStream);

        DataStreamProperties properties{};
        properties.set_type(DataStreamType::DataStreamTypeFixed);
        properties.set_pattern(DataStreamPattern::DataStreamPatternConstant);
        *properties.mutable_fixed() = std::move(fixedTypeProperties);

        DataStreamDownloadRequest request{};
        *request.mutable_properties() = std::move(properties);

        DataStreamReader dataStreamReader{ client.get(), &request };

        uint32_t numberOfDataBlocksReceived{};
        DataStreamOperationStatus operationStatus{};
        std::span<uint32_t> lostDataBlockSequenceNumbers{};
        const grpc::Status status = dataStreamReader.Await(&numberOfDataBlocksReceived, &operationStatus, lostDataBlockSequenceNumbers);
        REQUIRE(status.ok());
        REQUIRE(numberOfDataBlocksReceived == fixedNumberOfDataBlocksToStream);
        REQUIRE(operationStatus.code() == DataStreamOperationStatusCodeSucceeded);
        REQUIRE(lostDataBlockSequenceNumbers.empty());
    }

    SECTION("Can be called with DataStreamTypeContinuous and DataStreamPatternConstant")
    {
        static constexpr auto StreamingDelayTime = 5s;

        DataStreamContinuousTypeProperties continuousTypeProperties{};

        DataStreamProperties properties{};
        properties.set_type(DataStreamType::DataStreamTypeContinuous);
        properties.set_pattern(DataStreamPattern::DataStreamPatternConstant);
        *properties.mutable_continuous() = std::move(continuousTypeProperties);

        DataStreamDownloadRequest request{};
        *request.mutable_properties() = std::move(properties);

        DataStreamReader dataStreamReader{ client.get(), &request };

        // Allow some time of continuous streaming by the server, then cancel the RPC.
        std::this_thread::sleep_for(StreamingDelayTime);
        dataStreamReader.Cancel();

        uint32_t numberOfDataBlocksReceived{};
        DataStreamOperationStatus operationStatus{};
        std::span<uint32_t> lostDataBlockSequenceNumbers{};
        const grpc::Status status = dataStreamReader.Await(&numberOfDataBlocksReceived, &operationStatus, lostDataBlockSequenceNumbers);
        REQUIRE(status.error_code() == grpc::StatusCode::CANCELLED);
        REQUIRE(operationStatus.code() == DataStreamOperationStatusCodeSucceeded);
        REQUIRE(lostDataBlockSequenceNumbers.empty());
    }
}

TEST_CASE("DataStreamBidirectional API", "[basic][rpc][client][remote][stream]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::DataStream;
    using namespace Microsoft::Net::Remote::Service;

    using Microsoft::Net::Remote::Test::DataStreamReaderWriter;
    using Microsoft::Net::Remote::Test::RemoteServiceAddressHttp;

    const NetRemoteServerConfiguration Configuration{
        .ServerAddress = RemoteServiceAddressHttp,
    };

    NetRemoteServer server{ Configuration };
    server.Run();

    auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
    auto client = NetRemoteDataStreaming::NewStub(channel);

    static constexpr auto fixedNumberOfDataBlocksToStream = 10;

    SECTION("Can be called with DataStreamTypeFixed")
    {
        DataStreamFixedTypeProperties fixedTypeProperties{};
        fixedTypeProperties.set_numberofdatablockstostream(fixedNumberOfDataBlocksToStream);

        DataStreamProperties properties{};
        properties.set_type(DataStreamType::DataStreamTypeFixed);
        *properties.mutable_fixed() = std::move(fixedTypeProperties);

        DataStreamReaderWriter dataStreamReaderWriter{ client.get(), std::move(properties) };

        uint32_t numberOfDataBlocksReceived{};
        DataStreamOperationStatus operationStatus{};
        std::span<uint32_t> lostDataBlockSequenceNumbers{};
        const grpc::Status status = dataStreamReaderWriter.Await(&numberOfDataBlocksReceived, &operationStatus, lostDataBlockSequenceNumbers);
        REQUIRE(status.ok());
        REQUIRE(numberOfDataBlocksReceived > 0);
        REQUIRE(operationStatus.code() == DataStreamOperationStatusCodeSucceeded);
        REQUIRE(lostDataBlockSequenceNumbers.empty());
    }

    SECTION("Can be called with DataStreamTypeContinuous")
    {
        static constexpr auto StreamingDelayTime = 5s;

        DataStreamContinuousTypeProperties continuousTypeProperties{};

        DataStreamProperties properties{};
        properties.set_type(DataStreamType::DataStreamTypeContinuous);
        *properties.mutable_continuous() = std::move(continuousTypeProperties);

        DataStreamReaderWriter dataStreamReaderWriter{ client.get(), std::move(properties) };

        // Allow some time of continuous streaming, then stop writing data. This will prompt the
        // server to stop writing too, eliminating the need to cancel the RPC.
        std::this_thread::sleep_for(StreamingDelayTime);
        dataStreamReaderWriter.StopWrites();

        uint32_t numberOfDataBlocksReceived{};
        DataStreamOperationStatus operationStatus{};
        std::span<uint32_t> lostDataBlockSequenceNumbers{};
        const grpc::Status status = dataStreamReaderWriter.Await(&numberOfDataBlocksReceived, &operationStatus, lostDataBlockSequenceNumbers);
        REQUIRE(status.ok());
        REQUIRE(numberOfDataBlocksReceived > 0);
        REQUIRE(operationStatus.code() == DataStreamOperationStatusCodeSucceeded);
        REQUIRE(lostDataBlockSequenceNumbers.empty());
    }
}

TEST_CASE("DataStreamPing API", "[basic][rpc][client][remote][stream]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::DataStream;
    using namespace Microsoft::Net::Remote::Service;

    using Microsoft::Net::Remote::Test::DataStreamReaderWriter;
    using Microsoft::Net::Remote::Test::RemoteServiceAddressHttp;

    const NetRemoteServerConfiguration Configuration{
        .ServerAddress = RemoteServiceAddressHttp,
    };

    NetRemoteServer server{ Configuration };
    server.Run();

    auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
    auto client = NetRemoteDataStreaming::NewStub(channel);

    SECTION("Can be called")
    {
        grpc::ClientContext clientContext{};
        google::protobuf::Empty request{};
        google::protobuf::Empty response{};

        grpc::Status status = client->DataStreamPing(&clientContext, request, &response);
        REQUIRE(status.ok());
    }
}
