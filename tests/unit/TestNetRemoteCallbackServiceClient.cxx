
#include <condition_variable>
#include <memory>
#include <mutex>

#include <catch2/catch_test_macros.hpp>
#include <grpcpp/create_channel.h>
#include <magic_enum.hpp>
#include <microsoft/net/remote/NetRemoteServer.hxx>
#include <microsoft/net/remote/protocol/NetRemoteCallbackService.grpc.pb.h>

#include "TestNetRemoteCommon.hxx"

using Microsoft::Net::Remote::Test::RemoteServiceAddressHttp;

TEST_CASE("WifiDataStreamUpload API", "[basic][rpc][client][remote]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Remote::Test;
    using namespace Microsoft::Net::Remote::Wifi;
    using namespace Microsoft::Net::Wifi;

    NetRemoteServerConfiguration Configuration{
        .ServerAddress = RemoteServiceAddressHttp
    };

    NetRemoteServer server{ Configuration };
    server.Run();

    auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
    auto client = NetRemoteCallback::NewStub(channel);

    SECTION("Can be called")
    {
        class StreamWriter : public grpc::ClientWriteReactor<WifiDataStreamData>
        {
        public:
            StreamWriter(NetRemoteCallback::Stub* client, uint32_t dataToWriteCount) :
                m_dataToWriteCount(dataToWriteCount)
            {
                client->async()->WifiDataStreamUpload(&m_clientContext, &m_result, this);
                StartCall();
                NextWrite();
            }

            void OnWriteDone(bool ok) override
            {
                if (ok) {
                    NextWrite();
                } else {
                    StartWritesDone();
                }
            }

            void OnDone(const grpc::Status& status) override
            {
                std::unique_lock lock(m_mutex);

                m_status = status;
                m_done = true;
                m_cv.notify_one();
            }

            grpc::Status Await(WifiDataStreamUploadResult* result)
            {
                std::unique_lock lock(m_mutex);

                m_cv.wait(lock, [this] {
                    return m_done;
                });
                *result = m_result;

                return m_status;
            }

        private:
            void NextWrite()
            {
                if (m_dataToWriteCount > 0) {
                    WifiDataStreamData data{};
                    data.set_data("Data");
                    StartWrite(&data);
                    m_dataToWriteCount--;
                } else {
                    StartWritesDone();
                }
            }

        private:
            grpc::ClientContext m_clientContext{};
            WifiDataStreamUploadResult m_result{};
            uint32_t m_dataToWriteCount{};
            grpc::Status m_status{};
            std::mutex m_mutex{};
            std::condition_variable m_cv{};
            bool m_done{false};
        };

        static constexpr auto dataToWriteCount = 10;
        auto streamWriter = std::make_unique<StreamWriter>(client.get(), dataToWriteCount);

        WifiDataStreamUploadResult result{};
        grpc::Status status = streamWriter->Await(&result);
        REQUIRE(status.ok());
        REQUIRE(result.datareceivedcount() == dataToWriteCount);
        REQUIRE(result.status().code() == WifiDataStreamOperationStatusCodeSucceeded);
    }
}

TEST_CASE("WifiDataStreamDownload API", "[basic][rpc][client][remote]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Remote::Test;
    using namespace Microsoft::Net::Remote::Wifi;
    using namespace Microsoft::Net::Wifi;

    NetRemoteServerConfiguration Configuration{
        .ServerAddress = RemoteServiceAddressHttp
    };

    NetRemoteServer server{ Configuration };
    server.Run();

    auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
    auto client = NetRemoteCallback::NewStub(channel);

    SECTION("Can be called")
    {
        class StreamReader : public grpc::ClientReadReactor<WifiDataStreamData>
        {
        public:
            StreamReader(NetRemoteCallback::Stub* client, WifiDataStreamDownloadRequest* request)
            {
                client->async()->WifiDataStreamDownload(&m_clientContext, request, this);
                StartCall();
                StartRead(&m_data);
            }

            void OnReadDone(bool ok) override
            {
                if (ok) {
                    m_dataReceivedCount++;
                    StartRead(&m_data);
                }
                // If read fails, then there is likely no more data to be read, so do nothing.
            }

            void OnDone(const grpc::Status& status) override
            {
                std::unique_lock lock(m_mutex);

                m_status = status;
                m_done = true;
                m_cv.notify_one();
            }

            grpc::Status Await(uint32_t* dataReceivedCount)
            {
                std::unique_lock lock(m_mutex);

                m_cv.wait(lock, [this] {
                    return m_done;
                });
                *dataReceivedCount = m_dataReceivedCount;

                return m_status;
            }

        private:
            grpc::ClientContext m_clientContext{};
            uint32_t m_dataReceivedCount{};
            WifiDataStreamData m_data{};
            grpc::Status m_status{};
            std::mutex m_mutex{};
            std::condition_variable m_cv{};
            bool m_done{false};
        };

        static constexpr auto dataRequestedCount = 10;
        WifiDataStreamDownloadRequest request{};
        request.set_datarequestedcount(dataRequestedCount);
        auto streamReader = std::make_unique<StreamReader>(client.get(), &request);

        uint32_t dataReceivedCount{};
        grpc::Status status = streamReader->Await(&dataReceivedCount);
        REQUIRE(status.ok());
        REQUIRE(dataReceivedCount == dataRequestedCount);
    }
}
