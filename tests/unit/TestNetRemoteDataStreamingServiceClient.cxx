
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

TEST_CASE("WifiDataStreamUpload API", "[basic][rpc][client][remote]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Remote::Wifi;

    using Microsoft::Net::Remote::Test::RemoteServiceAddressHttp;

    NetRemoteServerConfiguration Configuration{
        .ServerAddress = RemoteServiceAddressHttp,
    };

    NetRemoteServer server{ Configuration };
    server.Run();

    auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
    auto client = NetRemoteDataStreaming::NewStub(channel);

    class DataStreamWriter : public grpc::ClientWriteReactor<WifiDataStreamUploadData>
    {
    public:
        DataStreamWriter(NetRemoteDataStreaming::Stub* client, uint32_t numberOfDataBlocksToWrite) :
            m_numberOfDataBlocksToWrite(numberOfDataBlocksToWrite)
        {
            client->async()->WifiDataStreamUpload(&m_clientContext, &m_result, this);
            StartCall();
            NextWrite();
        }

        void
        OnWriteDone(bool isOk) override
        {
            if (isOk) {
                NextWrite();
            } else {
                StartWritesDone();
            }
        }

        void
        OnDone(const grpc::Status& status) override
        {
            std::unique_lock lock(m_mutex);

            m_status = status;
            m_done = true;
            m_cv.notify_one();
        }

        grpc::Status
        Await(WifiDataStreamUploadResult* result)
        {
            std::unique_lock lock(m_mutex);

            m_cv.wait(lock, [this] {
                return m_done;
            });
            *result = m_result;

            return m_status;
        }

    private:
        void
        NextWrite()
        {
            if (m_numberOfDataBlocksToWrite > 0) {
                m_data.set_data(std::format("Data #{}", ++m_numberOfDataBlocksWritten));
                StartWrite(&m_data);
                m_numberOfDataBlocksToWrite--;
            } else {
                StartWritesDone();
            }
        }

    private:
        grpc::ClientContext m_clientContext{};
        WifiDataStreamUploadData m_data{};
        WifiDataStreamUploadResult m_result{};
        uint32_t m_numberOfDataBlocksToWrite{};
        uint32_t m_numberOfDataBlocksWritten{};
        grpc::Status m_status{};
        std::mutex m_mutex{};
        std::condition_variable m_cv{};
        bool m_done{ false };
    };

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
