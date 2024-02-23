
#ifndef NET_REMOTE_CALLBACK_SERVICE_HXX
#define NET_REMOTE_CALLBACK_SERVICE_HXX

#include <microsoft/net/remote/protocol/NetRemoteCallbackService.grpc.pb.h>

namespace Microsoft::Net::Remote::Service
{
/**
 * @brief Implementation of the NetRemoteCallback::CallbackService gRPC service.
 */
class NetRemoteCallbackService :
    public NetRemoteCallback::CallbackService
{
public:
    /**
     * @brief Construct a new NetRemoteCallbackService object.
     *
     */
    NetRemoteCallbackService() = default;

private:
    virtual ::grpc::ServerReadReactor<::Microsoft::Net::Remote::Wifi::WifiDataStreamUploadData>*
    WifiDataStreamUpload(::grpc::CallbackServerContext* context, ::Microsoft::Net::Remote::Wifi::WifiDataStreamUploadResult* result) override;

    virtual ::grpc::ServerWriteReactor<::Microsoft::Net::Remote::Wifi::WifiDataStreamDownloadData>*
    WifiDataStreamDownload(::grpc::CallbackServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiDataStreamDownloadRequest* request) override;

    virtual ::grpc::ServerBidiReactor<::Microsoft::Net::Remote::Wifi::WifiDataStreamUploadData, ::Microsoft::Net::Remote::Wifi::WifiDataStreamDownloadData>*
    WifiDataStreamBidirectional(::grpc::CallbackServerContext* context) override;
};
} // namespace Microsoft::Net::Remote::Service

#endif // NET_REMOTE_CALLBACK_SERVICE_HXX
