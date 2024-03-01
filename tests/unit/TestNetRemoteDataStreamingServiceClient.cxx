
#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <grpcpp/create_channel.h>
#include <microsoft/net/remote/NetRemoteServer.hxx>
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

    NetRemoteServerConfiguration Configuration{
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
        grpc::Status status = dataStreamWriter->Await(&result);
        REQUIRE(status.ok());
        REQUIRE(result.numberofdatablocksreceived() == numberOfDataBlocksToWrite);
        REQUIRE(result.status().code() == DataStreamOperationStatusCodeSucceeded);
    }

    SECTION("Can be called with multiple parallel clients")
    {
        static constexpr auto numberOfClients = 5;

        std::vector<std::unique_ptr<NetRemoteDataStreaming::Stub>> dataStreamingClients;
        std::vector<std::jthread> clientThreads;

        for (std::size_t i = 0; i < numberOfClients; i++) {
            dataStreamingClients.push_back(NetRemoteDataStreaming::NewStub(channel));
            auto dataStreamWriter = std::make_unique<DataStreamWriter>(dataStreamingClients.back().get(), numberOfDataBlocksToWrite);

            clientThreads.emplace_back([dataStreamWriter = std::move(dataStreamWriter)]() {
                DataStreamUploadResult result{};
                grpc::Status status = dataStreamWriter->Await(&result);
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

    NetRemoteServerConfiguration Configuration{
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
        grpc::Status status = dataStreamReader.Await(&numberOfDataBlocksReceived, &operationStatus);
        REQUIRE(status.ok());
        REQUIRE(numberOfDataBlocksReceived == fixedNumberOfDataBlocksToStream);
        REQUIRE(operationStatus.code() == DataStreamOperationStatusCodeSucceeded);
    }

    SECTION("Can be called with DataStreamTypeContinuous and DataStreamPatternConstant")
    {
        DataStreamContinuousTypeProperties continuousTypeProperties{};

        DataStreamProperties properties{};
        properties.set_type(DataStreamType::DataStreamTypeContinuous);
        properties.set_pattern(DataStreamPattern::DataStreamPatternConstant);
        *properties.mutable_continuous() = std::move(continuousTypeProperties);

        DataStreamDownloadRequest request{};
        *request.mutable_properties() = std::move(properties);

        DataStreamReader dataStreamReader{ client.get(), &request };

        // Allow some time of continuous streaming by the server, then cancel the RPC.
        std::this_thread::sleep_for(5s);
        dataStreamReader.Cancel();

        uint32_t numberOfDataBlocksReceived{};
        DataStreamOperationStatus operationStatus{};
        grpc::Status status = dataStreamReader.Await(&numberOfDataBlocksReceived, &operationStatus);
        REQUIRE(status.error_code() == grpc::StatusCode::CANCELLED);
        REQUIRE(operationStatus.code() == DataStreamOperationStatusCodeSucceeded);
    }
}
