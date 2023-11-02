
#ifndef WIFI_REMOTE_SERVICE_IMPL_HXX
#define WIFI_REMOTE_SERVICE_IMPL_HXX

#include <WifiRemoteService.grpc.pb.h>

namespace Microsoft::Wifi::Remote::Service
{
class WifiRemoteServiceImpl : public WifiRemoteService::Service
{
    virtual ::grpc::Status GetWifiApControl(::grpc::ServerContext* context, const ::Microsoft::Wifi::Remote::Request* request, ::Microsoft::Wifi::Remote::Response* response) override;
};
} // namespace Microsoft::Wifi::Remote::Service

#endif // WIFI_REMOTE_SERVICE_IMPL_HXX 
