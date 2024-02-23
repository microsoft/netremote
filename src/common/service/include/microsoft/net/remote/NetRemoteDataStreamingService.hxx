
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
};
} // namespace Microsoft::Net::Remote::Service

#endif // NET_REMOTE_DATA_STREAMING_SERVICE_HXX
