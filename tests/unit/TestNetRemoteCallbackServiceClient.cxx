
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
        class StreamWriter : public grpc::ClientWriteReactor<WifiDataStreamUploadData>
        {
        public:
            StreamWriter(NetRemoteCallback::Stub* client, uint32_t dataToWriteCount) :
                m_dataToWriteCount(dataToWriteCount)
            {
                client->async()->WifiDataStreamUpload(&m_clientContext, &m_result, this);
                StartCall();
                NextWrite();
            }

            void
            OnWriteDone(bool ok) override
            {
                if (ok) {
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
                if (m_dataToWriteCount > 0) {
                    m_data.set_data(std::format("Data #{}", ++m_dataSentCount));
                    StartWrite(&m_data);
                    m_dataToWriteCount--;
                } else {
                    StartWritesDone();
                }
            }

        private:
            grpc::ClientContext m_clientContext{};
            WifiDataStreamUploadData m_data{};
            WifiDataStreamUploadResult m_result{};
            uint32_t m_dataToWriteCount{};
            uint32_t m_dataSentCount{};
            grpc::Status m_status{};
            std::mutex m_mutex{};
            std::condition_variable m_cv{};
            bool m_done{ false };
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

    SECTION("Can be called with WifiDataStreamType::Fixed")
    {
        class StreamReader : public grpc::ClientReadReactor<WifiDataStreamDownloadData>
        {
        public:
            StreamReader(NetRemoteCallback::Stub* client, WifiDataStreamDownloadRequest* request)
            {
                client->async()->WifiDataStreamDownload(&m_clientContext, request, this);
                StartCall();
                StartRead(&m_data);
            }

            void
            OnReadDone(bool ok) override
            {
                if (ok) {
                    REQUIRE(m_data.sequencenumber() == ++m_dataReceivedCount);
                    StartRead(&m_data);
                }
                // If read fails, then there is likely no more data to be read, so do nothing.
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
            Await(uint32_t* dataReceivedCount, WifiDataStreamOperationStatus* operationStatus)
            {
                std::unique_lock lock(m_mutex);

                m_cv.wait(lock, [this] {
                    return m_done;
                });
                *dataReceivedCount = m_dataReceivedCount;
                *operationStatus = m_data.status();

                return m_status;
            }

        private:
            grpc::ClientContext m_clientContext{};
            WifiDataStreamDownloadData m_data{};
            uint32_t m_dataReceivedCount{};
            grpc::Status m_status{};
            std::mutex m_mutex{};
            std::condition_variable m_cv{};
            bool m_done{ false };
        };

        static constexpr auto dataRequestedCount = 10;
        WifiDataStreamDownloadRequest request{};

        WifiDataStreamProperties streamProperties{};
        streamProperties.set_type(WifiDataStreamType::Fixed);

        WifiDataStreamFixedTypeProperties fixedTypeProperties{};
        fixedTypeProperties.set_numberofdatablockstostream(dataRequestedCount);

        *streamProperties.mutable_fixedtypeproperties() = std::move(fixedTypeProperties);

        *request.mutable_properties() = std::move(streamProperties);
        auto streamReader = std::make_unique<StreamReader>(client.get(), &request);

        uint32_t dataReceivedCount{};
        WifiDataStreamOperationStatus operationStatus{};
        grpc::Status status = streamReader->Await(&dataReceivedCount, &operationStatus);
        REQUIRE(status.ok());
        REQUIRE(dataReceivedCount == dataRequestedCount);
        REQUIRE(operationStatus.code() == WifiDataStreamOperationStatusCode::WifiDataStreamOperationStatusCodeSucceeded);
    }
}

TEST_CASE("WifiDataStreamBidirectional API", "[basic][rpc][client][remote]")
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
        class StreamReaderWriter : public grpc::ClientBidiReactor<WifiDataStreamUploadData, WifiDataStreamDownloadData>
        {
        public:
            StreamReaderWriter(NetRemoteCallback::Stub* client, uint32_t dataToWriteCount) :
                m_dataToWriteCount(dataToWriteCount)
            {
                client->async()->WifiDataStreamBidirectional(&m_clientContext, this);
                StartCall();
                StartRead(&m_readData);
                NextWrite();
            }

            void
            OnReadDone(bool ok) override
            {
                if (ok) {
                    REQUIRE(m_readData.sequencenumber() == ++m_dataReceivedCount);
                    StartRead(&m_readData);
                }
            }

            void
            OnWriteDone(bool ok) override
            {
                if (ok) {
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
            Await(WifiDataStreamOperationStatus* operationStatus)
            {
                std::unique_lock lock(m_mutex);

                m_cv.wait(lock, [this] {
                    return m_done;
                });
                *operationStatus = m_readData.status();

                return m_status;
            }

        private:
            void
            NextWrite()
            {
                if (m_dataToWriteCount > 0) {
                    m_writeData.set_data(std::format("Data #{}", ++m_dataSentCount));
                    StartWrite(&m_writeData);
                    m_dataToWriteCount--;
                } else {
                    StartWritesDone();
                }
            }

        private:
            grpc::ClientContext m_clientContext{};
            WifiDataStreamDownloadData m_readData{};
            WifiDataStreamUploadData m_writeData{};
            uint32_t m_dataToWriteCount{};
            uint32_t m_dataSentCount{};
            uint32_t m_dataReceivedCount{};
            grpc::Status m_status{};
            std::mutex m_mutex{};
            std::condition_variable m_cv{};
            bool m_done{ false };
        };

        static constexpr auto dataToWriteCount = 10;
        auto streamReaderWriter = std::make_unique<StreamReaderWriter>(client.get(), dataToWriteCount);

        WifiDataStreamOperationStatus operationStatus{};
        grpc::Status status = streamReaderWriter->Await(&operationStatus);
        REQUIRE(status.ok());
        REQUIRE(operationStatus.code() == WifiDataStreamOperationStatusCodeSucceeded);
    }
}
