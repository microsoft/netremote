
#ifndef NET_REMOTE_SERVICE_HXX
#define NET_REMOTE_SERVICE_HXX

#include <string_view>

#include <microsoft/net/remote/protocol/NetRemoteService.grpc.pb.h>

namespace Microsoft::Net::Remote::Service
{
class NetRemoteService :
    public NetRemote::Service
{
public:
    virtual ::grpc::Status
    WifiConfigureAccessPoint(::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiAccessPointApplyConfigurationRequest* request, ::Microsoft::Net::Remote::Wifi::WifiAccessPointApplyConfigurationResult* response) override;

    virtual ::grpc::Status
    WifiEnumerateAccessPoints(::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsRequest* request, ::Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsResult* response) override;

protected:
    ::Microsoft::Net::Remote::Wifi::WifiAccessPointApplyConfigurationResultConfigurationItem
    WifiAccessPointApplyConfiguration(std::string_view accessPointId, const ::Microsoft::Net::Remote::Wifi::WifiAccessPointApplyConfigurationRequestConfigurationItem& configuredDesired);
};
} // namespace Microsoft::Net::Remote::Service

#endif // NET_REMOTE_SERVICE_HXX
