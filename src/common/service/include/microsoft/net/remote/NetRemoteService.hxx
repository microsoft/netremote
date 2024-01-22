
#ifndef NET_REMOTE_SERVICE_HXX
#define NET_REMOTE_SERVICE_HXX

#include <memory>

#include <microsoft/net/remote/protocol/NetRemoteService.grpc.pb.h>
#include <microsoft/net/wifi/AccessPointManager.hxx>

namespace Microsoft::Net::Remote::Service
{
/**
 * @brief Implementation of the NetRemote::Service gRPC service.
 */
class NetRemoteService :
    public NetRemote::Service
{
public:
    /**
     * @brief Construct a new NetRemoteService object with the specified access point manager.
     *
     * @param accessPointManager The access point manager to use.
     */
    NetRemoteService(std::shared_ptr<Microsoft::Net::Wifi::AccessPointManager> accessPointManager);

    /**
     * @brief Get the AccessPointManager object for this service.
     *
     * @return std::shared_ptr<Microsoft::Net::Wifi::AccessPointManager>
     */
    std::shared_ptr<Microsoft::Net::Wifi::AccessPointManager>
    GetAccessPointManager() noexcept;

private:
    virtual ::grpc::Status
    WifiEnumerateAccessPoints(::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsRequest* request, ::Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsResult* response) override;

    virtual ::grpc::Status
    WifiAccessPointEnable(::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiAccessPointEnableRequest* request, ::Microsoft::Net::Remote::Wifi::WifiAccessPointEnableResult* response) override;

    virtual ::grpc::Status
    WifiAccessPointDisable(::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiAccessPointDisableRequest* request, ::Microsoft::Net::Remote::Wifi::WifiAccessPointDisableResult* response) override;

    virtual ::grpc::Status
    WifiAccessPointSetPhyType(::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiAccessPointSetPhyTypeRequest* request, ::Microsoft::Net::Remote::Wifi::WifiAccessPointSetPhyTypeResult* response) override;

    virtual ::grpc::Status
    WifiAccessPointSetAuthenticationMethod(::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiAccessPointSetAuthenticationMethodRequest* request, ::Microsoft::Net::Remote::Wifi::WifiAccessPointSetAuthenticationMethodResult* response) override;

    virtual ::grpc::Status
    WifiAccessPointSetEncryptionMethod(::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiAccessPointSetEncryptionMethodRequest* request, ::Microsoft::Net::Remote::Wifi::WifiAccessPointSetEncryptionMethodResult* response) override;

protected:
    bool
    ValidateWifiAccessPointEnableRequest(const ::Microsoft::Net::Remote::Wifi::WifiAccessPointEnableRequest* request, ::Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus& status);

private:
    std::shared_ptr<Microsoft::Net::Wifi::AccessPointManager> m_accessPointManager;
};
} // namespace Microsoft::Net::Remote::Service

#endif // NET_REMOTE_SERVICE_HXX
