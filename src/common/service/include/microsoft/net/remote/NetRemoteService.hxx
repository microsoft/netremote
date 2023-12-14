
#ifndef NET_REMOTE_SERVICE_HXX
#define NET_REMOTE_SERVICE_HXX

#include <microsoft/net/remote/protocol/NetRemoteService.grpc.pb.h>

namespace Microsoft::Net::Remote::Service
{
class NetRemoteService : public NetRemote::Service
{
    virtual ::grpc::Status WifiConfigureAccessPoint(::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiConfigureAccessPointRequest* request, ::Microsoft::Net::Remote::Wifi::WifiConfigureAccessPointResult* response) override;
    virtual ::grpc::Status WifiEnumerateAccessPoints(::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsRequest* request, ::Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsResult* response) override;
};
} // namespace Microsoft::Net::Remote::Service

#endif // NET_REMOTE_SERVICE_HXX 
