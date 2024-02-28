
#ifndef NET_REMOTE_DATA_STREAMING_SERVICE_HXX
#define NET_REMOTE_DATA_STREAMING_SERVICE_HXX

#include <microsoft/net/remote/protocol/NetRemoteDataStream.pb.h>
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
     * @return grpc::ServerReadReactor<Microsoft::Net::Remote::DataStream::DataStreamUploadData>*
     */
    grpc::ServerReadReactor<Microsoft::Net::Remote::DataStream::DataStreamUploadData>*
    DataStreamUpload(grpc::CallbackServerContext* context, Microsoft::Net::Remote::DataStream::DataStreamUploadResult* result) override;

    /**
     * @brief Stream data from the server to the client.
     *
     * @param context
     * @param request
     * @return grpc::ServerWriteReactor<Microsoft::Net::Remote::DataStream::DataStreamDownloadData>*
     */
    grpc::ServerWriteReactor<Microsoft::Net::Remote::DataStream::DataStreamDownloadData>*
    DataStreamDownload(grpc::CallbackServerContext* context, const Microsoft::Net::Remote::DataStream::DataStreamDownloadRequest* request) override;
};
} // namespace Microsoft::Net::Remote::Service

#endif // NET_REMOTE_DATA_STREAMING_SERVICE_HXX
