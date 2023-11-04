
#ifndef NET_REMOTE_SERVICE_IMPL_HXX
#define NET_REMOTE_SERVICE_IMPL_HXX

#include <NetRemoteService.grpc.pb.h>

namespace Microsoft::Net::Remote::Service
{
class NetRemoteService : public NetRemote::Service
{
    virtual ::grpc::Status GetWifiApControl(::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Request* request, ::Microsoft::Net::Remote::Response* response) override;
};
} // namespace Microsoft::Net::Remote::Service

#endif // NET_REMOTE_SERVICE_IMPL_HXX 
