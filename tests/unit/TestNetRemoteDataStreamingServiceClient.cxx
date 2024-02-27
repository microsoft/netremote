
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <grpcpp/create_channel.h>
#include <microsoft/net/remote/NetRemoteServer.hxx>
#include <microsoft/net/remote/protocol/NetRemoteDataStreamingService.grpc.pb.h>

#include "TestNetRemoteCommon.hxx"
#include "TestNetRemoteDataStreamingReactors.hxx"

TEST_CASE("WifiDataStreamUpload API", "[basic][rpc][client][remote][stream]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Remote::Wifi;

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

        WifiDataStreamUploadResult result{};
        grpc::Status status = dataStreamWriter->Await(&result);
        REQUIRE(status.ok());
        REQUIRE(result.numberofdatablocksreceived() == numberOfDataBlocksToWrite);
        REQUIRE(result.status().code() == WifiDataStreamOperationStatusCodeSucceeded);
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
                WifiDataStreamUploadResult result{};
                grpc::Status status = dataStreamWriter->Await(&result);
                REQUIRE(status.ok());
                REQUIRE(result.numberofdatablocksreceived() == numberOfDataBlocksToWrite);
                REQUIRE(result.status().code() == WifiDataStreamOperationStatusCodeSucceeded);
            });
        }
    }
}