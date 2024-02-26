
#ifndef NET_REMOTE_DATA_STREAMING_SERVICE_HXX
#define NET_REMOTE_DATA_STREAMING_SERVICE_HXX

#include <microsoft/net/remote/protocol/NetRemoteDataStreamingService.grpc.pb.h>

namespace Microsoft::Net::Remote::Service
{
/**
 * @brief Implementation of the NetRemoteDataStreaming::CallbackService gRPC service.
 */
class NetRemoteDataStreamingService :
    public NetRemoteDataStreaming::CallbackService
{
public:
    /**
     * @brief Construct a new NetRemoteDataStreamingService object.
     */
    NetRemoteDataStreamingService() = default;

private:
    /**
     * @brief Stream data from the client to the server.
     *
     * @param context
     * @param result
     * @return grpc::ServerReadReactor<Microsoft::Net::Remote::Wifi::WifiDataStreamUploadData>*
     */
    grpc::ServerReadReactor<Microsoft::Net::Remote::Wifi::WifiDataStreamUploadData>*
    WifiDataStreamUpload(grpc::CallbackServerContext* context, Microsoft::Net::Remote::Wifi::WifiDataStreamUploadResult* result) override;
};
} // namespace Microsoft::Net::Remote::Service

#endif // NET_REMOTE_DATA_STREAMING_SERVICE_HXX
