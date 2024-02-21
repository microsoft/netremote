#include <microsoft/net/remote/NetRemoteCallbackService.hxx>
#include <plog/Log.h>

using namespace Microsoft::Net::Remote::Service;

using Microsoft::Net::Remote::Wifi::WifiDataStreamOperationStatus;
using Microsoft::Net::Remote::Wifi::WifiDataStreamOperationStatusCode;

::grpc::ServerReadReactor<::Microsoft::Net::Remote::Wifi::WifiDataStreamUploadData>*
NetRemoteCallbackService::WifiDataStreamUpload([[maybe_unused]] ::grpc::CallbackServerContext* context, ::Microsoft::Net::Remote::Wifi::WifiDataStreamUploadResult* result)
{
    LOGD << "Received WifiDataStreamUpload request";

    class StreamReader : public ::grpc::ServerReadReactor<::Microsoft::Net::Remote::Wifi::WifiDataStreamUploadData>
    {
    public:
        StreamReader(::Microsoft::Net::Remote::Wifi::WifiDataStreamUploadResult* result) :
            m_result(result)
        {
            StartRead(&m_data);
        }

        void
        OnReadDone(bool ok) override
        {
            if (ok) {
                LOGD << "Data read successful";
                m_dataReceivedCount++;
                m_readStatus.set_code(WifiDataStreamOperationStatusCode::WifiDataStreamOperationStatusCodeSucceeded);
                m_readStatus.set_message("Data read successful");
                StartRead(&m_data);
            } else {
                // A false "ok" value could either mean a failed RPC or that no more data is available.
                // Unfortunately, there is no clear way to tell which situation occurred.
                LOGD << "Data read failed (RPC failed OR no more data)";

                m_result->set_datareceivedcount(m_dataReceivedCount);
                *m_result->mutable_status() = std::move(m_readStatus);
                Finish(::grpc::Status::OK);
            }
        }

        void
        OnCancel() override
        {
            LOGE << "RPC cancelled";
            m_result->set_datareceivedcount(m_dataReceivedCount);
            *m_result->mutable_status() = std::move(m_readStatus);
            Finish(::grpc::Status::CANCELLED);
        }

        void
        OnDone() override
        {
            delete this;
        }

    private:
        ::Microsoft::Net::Remote::Wifi::WifiDataStreamUploadData m_data{};
        ::Microsoft::Net::Remote::Wifi::WifiDataStreamUploadResult* m_result{};
        uint32_t m_dataReceivedCount{};
        ::Microsoft::Net::Remote::Wifi::WifiDataStreamOperationStatus m_readStatus{};
    };

    return new StreamReader(result);
}

using Microsoft::Net::Remote::Wifi::WifiDataStreamType;

::grpc::ServerWriteReactor<::Microsoft::Net::Remote::Wifi::WifiDataStreamDownloadData>*
NetRemoteCallbackService::WifiDataStreamDownload([[maybe_unused]] ::grpc::CallbackServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiDataStreamDownloadRequest* request)
{
    LOGD << "Received WifiDataStreamDownload request";

    class StreamWriter : public ::grpc::ServerWriteReactor<::Microsoft::Net::Remote::Wifi::WifiDataStreamDownloadData>
    {
    public:
        StreamWriter(const ::Microsoft::Net::Remote::Wifi::WifiDataStreamDownloadRequest* request)
        {
            m_streamProperties = request->properties();
            if (m_streamProperties.type() == WifiDataStreamType::Fixed) {
                // TODO: Ensure that m_streamProperties.Value_case() == WifiDataStreamProperties.kFixedTypeProperties
                m_dataRequestedCount = m_streamProperties.fixedtypeproperties().numberofdatablockstostream();
            }
            m_writeStatus.set_code(WifiDataStreamOperationStatusCode::WifiDataStreamOperationStatusCodeUnknown);
            m_writeStatus.set_message("No data sent yet");
            NextWrite();
        }

        void
        OnWriteDone(bool ok) override
        {
            // Check for a failed status code from HandleWriteFailure since that invoked a final write, thus causing this callback to be invoked.
            if (m_writeStatus.code() == WifiDataStreamOperationStatusCode::WifiDataStreamOperationStatusCodeFailed) {
                Finish(::grpc::Status::OK);
                return;
            }

            if (ok) {
                LOGD << "Data write successful";
                if (m_streamProperties.type() == WifiDataStreamType::Fixed) {
                    m_dataRequestedCount--;
                }
                m_writeStatus.set_code(WifiDataStreamOperationStatusCode::WifiDataStreamOperationStatusCodeSucceeded);
                m_writeStatus.set_message("Data write successful");
                NextWrite();
            } else {
                LOGE << "Data write failed";
                HandleWriteFailure();
            }
        }

        // TODO: Implement OnCancel() to handle continuous streaming

        void
        OnDone() override
        {
            delete this;
        }

    private:
        void
        NextWrite()
        {
            if (m_streamProperties.type() == WifiDataStreamType::Continuous || m_dataRequestedCount > 0) {
                const auto data = std::format("Data #{}", ++m_dataSentCount);
                LOGD << "Writing " << data;
                m_data.set_data(data);
                m_data.set_count(m_dataSentCount);
                *m_data.mutable_status() = m_writeStatus;
                StartWrite(&m_data);
            } else {
                // No more data to write
                Finish(::grpc::Status::OK);
            }
        }

        void
        HandleWriteFailure()
        {
            LOGD << "HandleWriteFailure called";
            m_writeStatus.set_code(WifiDataStreamOperationStatusCode::WifiDataStreamOperationStatusCodeFailed);
            m_writeStatus.set_message("Data write failed");
            StartWrite(&m_data);
        }

    private:
        ::Microsoft::Net::Remote::Wifi::WifiDataStreamDownloadData m_data{};
        ::Microsoft::Net::Remote::Wifi::WifiDataStreamProperties m_streamProperties{};
        uint32_t m_dataRequestedCount{};
        uint32_t m_dataSentCount{};
        ::Microsoft::Net::Remote::Wifi::WifiDataStreamOperationStatus m_writeStatus{};
    };

    return new StreamWriter(request);
}

::grpc::ServerBidiReactor<::Microsoft::Net::Remote::Wifi::WifiDataStreamUploadData, ::Microsoft::Net::Remote::Wifi::WifiDataStreamDownloadData>*
NetRemoteCallbackService::WifiDataStreamBidirectional([[maybe_unused]] ::grpc::CallbackServerContext* context)
{
    LOGD << "Received WifiDataStreamBidirectional request";

    class StreamReaderWriter : public ::grpc::ServerBidiReactor<::Microsoft::Net::Remote::Wifi::WifiDataStreamUploadData, ::Microsoft::Net::Remote::Wifi::WifiDataStreamDownloadData>
    {
    public:
        StreamReaderWriter()
        {
            m_status.set_code(WifiDataStreamOperationStatusCode::WifiDataStreamOperationStatusCodeUnknown);
            m_status.set_message("No data sent yet");

            StartRead(&m_readData);
            NextWrite();
        }

        void
        OnReadDone(bool ok) override
        {
            if (ok) {
                LOGD << "Data read successful";
                m_dataReceivedCount++;
                m_status.set_code(WifiDataStreamOperationStatusCode::WifiDataStreamOperationStatusCodeSucceeded);
                m_status.set_message("Data read successful");
                StartRead(&m_readData);
            } else {
                LOGE << "Data read failed";
                Finish(::grpc::Status::OK);
            }
        }

        void
        OnWriteDone(bool ok) override
        {
            // Check for a failed status code from HandleFailure since that invoked a final write, thus causing this callback to be invoked.
            if (m_status.code() == WifiDataStreamOperationStatusCode::WifiDataStreamOperationStatusCodeFailed) {
                Finish(::grpc::Status::OK);
                return;
            }

            if (ok) {
                LOGD << "Data write successful";
                m_status.set_code(WifiDataStreamOperationStatusCode::WifiDataStreamOperationStatusCodeSucceeded);
                m_status.set_message("Data write successful");
                NextWrite();
            } // else {
              // LOGE << "Data write failed";
              // HandleFailure("Data write failed");
            //}
        }

        void
        OnCancel() override
        {
            LOGE << "RPC cancelled";
            Finish(::grpc::Status::CANCELLED);
        }

        void
        OnDone() override
        {
            delete this;
        }

    private:
        void
        NextWrite()
        {
            const auto data = std::format("Data #{}", ++m_dataSentCount);
            LOGD << "Writing " << data;
            m_writeData.set_data(data);
            m_writeData.set_count(m_dataSentCount);
            *m_writeData.mutable_status() = m_status;
            StartWrite(&m_writeData);
        }

        void
        HandleFailure(const std::string failureMessage)
        {
            LOGD << "HandleFailure called";
            m_status.set_code(WifiDataStreamOperationStatusCode::WifiDataStreamOperationStatusCodeFailed);
            m_status.set_message(failureMessage);
            StartWrite(&m_writeData);
        }

    private:
        ::Microsoft::Net::Remote::Wifi::WifiDataStreamUploadData m_readData{};
        ::Microsoft::Net::Remote::Wifi::WifiDataStreamDownloadData m_writeData{};
        uint32_t m_dataReceivedCount{};
        uint32_t m_dataSentCount{};
        ::Microsoft::Net::Remote::Wifi::WifiDataStreamOperationStatus m_status{};
    };

    return new StreamReaderWriter();
}
