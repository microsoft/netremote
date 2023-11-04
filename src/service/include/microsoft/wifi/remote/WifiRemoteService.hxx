
#ifndef WIFI_REMOTE_SERVICE_IMPL_HXX
#define WIFI_REMOTE_SERVICE_IMPL_HXX

#include <WifiRemoteService.grpc.pb.h>

namespace Microsoft::Net::Wifi::Remote::Service
{
class WifiRemoteService : public WifiRemote::Service
{
    virtual ::grpc::Status GetWifiApControl(::grpc::ServerContext* context, const ::Microsoft::Net::Wifi::Remote::Request* request, ::Microsoft::Net::Wifi::Remote::Response* response) override;
};
} // namespace Microsoft::Net::Wifi::Remote::Service

#endif // WIFI_REMOTE_SERVICE_IMPL_HXX 
