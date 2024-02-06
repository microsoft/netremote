#include <microsoft/net/remote/NetRemoteCallbackService.hxx>
#include <plog/Log.h>

using namespace Microsoft::Net::Remote::Service;

using Microsoft::Net::Remote::Wifi::WifiDataStreamOperationStatus;
using Microsoft::Net::Remote::Wifi::WifiDataStreamOperationStatusCode;

::grpc::ServerReadReactor<::Microsoft::Net::Remote::Wifi::WifiDataStreamData>*
NetRemoteCallbackService::WifiDataStreamUpload([[maybe_unused]] ::grpc::CallbackServerContext* context, ::Microsoft::Net::Remote::Wifi::WifiDataStreamUploadResult* result)
{
    LOGD << "Received WifiDataStreamUpload request";

    class StreamReader : public ::grpc::ServerReadReactor<::Microsoft::Net::Remote::Wifi::WifiDataStreamData>
    {
    public:
        StreamReader(::Microsoft::Net::Remote::Wifi::WifiDataStreamUploadResult* result) :
            m_result(result)
        {
            StartRead(&m_data);
        }

        void OnReadDone(bool ok) override
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

        void OnCancel() override
        {
            LOGE << "RPC cancelled";
            m_result->set_datareceivedcount(m_dataReceivedCount);
            *m_result->mutable_status() = std::move(m_readStatus);
            Finish(::grpc::Status::CANCELLED);
        }

        void OnDone() override
        {
            delete this;
        }

    private:
        ::Microsoft::Net::Remote::Wifi::WifiDataStreamData m_data{};
        ::Microsoft::Net::Remote::Wifi::WifiDataStreamUploadResult* m_result{};
        uint32_t m_dataReceivedCount{};
        ::Microsoft::Net::Remote::Wifi::WifiDataStreamOperationStatus m_readStatus{};
    };

    return new StreamReader(result);
}
