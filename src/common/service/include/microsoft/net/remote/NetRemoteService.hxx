
#ifndef NET_REMOTE_SERVICE_HXX
#define NET_REMOTE_SERVICE_HXX

#include <memory>

#include <microsoft/net/remote/protocol/NetRemoteService.grpc.pb.h>
#include <microsoft/net/wifi/AccessPointManager.hxx>

namespace Microsoft::Net::Remote::Service
{
class NetRemoteService :
    public NetRemote::Service
{
    NetRemoteService();

    virtual ::grpc::Status
    WifiEnumerateAccessPoints(::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsRequest* request, ::Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsResult* response) override;

    virtual ::grpc::Status
    WifiAccessPointEnable(::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiAccessPointEnableRequest* request, ::Microsoft::Net::Remote::Wifi::WifiAccessPointEnableResult* response) override;

    virtual ::grpc::Status
    WifiAccessPointDisable(::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiAccessPointDisableRequest* request, ::Microsoft::Net::Remote::Wifi::WifiAccessPointDisableResult* response) override;

protected:
    bool
    ValidateWifiAccessPointEnableRequest(const ::Microsoft::Net::Remote::Wifi::WifiAccessPointEnableRequest* request, ::Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus& status);

private:
    std::shared_ptr<Microsoft::Net::Wifi::AccessPointManager> m_accessPointManager;
};
} // namespace Microsoft::Net::Remote::Service

#endif // NET_REMOTE_SERVICE_HXX
