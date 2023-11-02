
#ifndef WIFI_AP_CONTROL_SERVICE_IMPL_HXX
#define WIFI_AP_CONTROL_SERVICE_IMPL_HXX

#include <WifiApControlService.grpc.pb.h>

namespace Microsoft::Wifi::Test::ApControl::Service
{
class WifiApControlServiceImpl : public WifiApControlService::Service
{
    virtual ::grpc::Status GetWifiApControl(::grpc::ServerContext* context, const ::Microsoft::Wifi::Test::ApControl::Request* request, ::Microsoft::Wifi::Test::ApControl::Response* response) override;
};
} // namespace Microsoft::Wifi::Test::ApControl::Service

#endif // WIFI_AP_CONTROL_SERVICE_HX 
