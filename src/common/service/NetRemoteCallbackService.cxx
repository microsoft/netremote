#include <microsoft/net/remote/NetRemoteCallbackService.hxx>
#include <plog/Log.h>

using namespace Microsoft::Net::Remote::Service;

using Microsoft::Net::Remote::Wifi::WifiDataStreamOperationStatus;
using Microsoft::Net::Remote::Wifi::WifiDataStreamOperationStatusCode;

::grpc::ServerReadReactor<::Microsoft::Net::Remote::Wifi::WifiDataStreamUploadRequest>*
NetRemoteCallbackService::WifiDataStreamUpload(::grpc::CallbackServerContext* context, ::Microsoft::Net::Remote::Wifi::WifiDataStreamUploadResult* response)
{
    LOGD << "Received WifiDataStreamUpload request";

    ::grpc::ServerReadReactor<::Microsoft::Net::Remote::Wifi::WifiDataStreamUploadRequest>* readReactor{};
    return readReactor;
}
