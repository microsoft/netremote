
#ifndef NET_REMOTE_SERVICE_HXX
#define NET_REMOTE_SERVICE_HXX

#include <memory>

#include <grpcpp/server_context.h>
#include <grpcpp/support/status.h>
#include <microsoft/net/remote/protocol/NetRemoteService.grpc.pb.h>
#include <microsoft/net/remote/protocol/NetRemoteWifi.pb.h>
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
    explicit NetRemoteService(std::shared_ptr<Microsoft::Net::Wifi::AccessPointManager> accessPointManager);

    /**
     * @brief Get the AccessPointManager object for this service.
     *
     * @return std::shared_ptr<Microsoft::Net::Wifi::AccessPointManager>
     */
    std::shared_ptr<Microsoft::Net::Wifi::AccessPointManager>
    GetAccessPointManager() noexcept;

private:
    /**
     * @brief Enumerate the available access points.
     *
     * @param context
     * @param request
     * @param response
     * @return grpc::Status
     */
    grpc::Status
    WifiEnumerateAccessPoints(grpc::ServerContext* context, const Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsRequest* request, Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsResult* response) override;

    /**
     * @brief Enable an access point. This brings the access point online, making it available for use by clients.
     *
     * @param context
     * @param request
     * @param response
     * @return grpc::Status
     */
    grpc::Status
    WifiAccessPointEnable(grpc::ServerContext* context, const Microsoft::Net::Remote::Wifi::WifiAccessPointEnableRequest* request, Microsoft::Net::Remote::Wifi::WifiAccessPointEnableResult* result) override;

    /**
     * @brief Disable an access point. This will take the access point offline, making it unavailable for use by clients.
     *
     * @param context
     * @param request
     * @param result
     * @return grpc::Status
     */
    grpc::Status
    WifiAccessPointDisable(grpc::ServerContext* context, const Microsoft::Net::Remote::Wifi::WifiAccessPointDisableRequest* request, Microsoft::Net::Remote::Wifi::WifiAccessPointDisableResult* result) override;

    /**
     * @brief Set the active PHY type or protocol of the access point. The access point must be enabled. This will cause
     * the access point to temporarily go offline while the change is being applied.
     *
     * @param context
     * @param request
     * @param result
     * @return grpc::Status
     */
    grpc::Status
    WifiAccessPointSetPhyType(grpc::ServerContext* context, const Microsoft::Net::Remote::Wifi::WifiAccessPointSetPhyTypeRequest* request, Microsoft::Net::Remote::Wifi::WifiAccessPointSetPhyTypeResult* result) override;

    /**
     * @brief Set the active frequency bands of the access point. The access point must be enabled. This will cause
     * the access point to temporarily go offline while the change is being applied.
     *
     * @param context
     * @param request
     * @param result
     * @return grpc::Status
     */
    grpc::Status
    WifiAccessPointSetFrequencyBands(grpc::ServerContext* context, const Microsoft::Net::Remote::Wifi::WifiAccessPointSetFrequencyBandsRequest* request, Microsoft::Net::Remote::Wifi::WifiAccessPointSetFrequencyBandsResult* result) override;

protected:
    /**
     * @brief Validate the basic input parameters for the WifiAccessPointEnable request.
     *
     * @param request The request to validate.
     * @param status The status to populate with failure information.
     * @return true
     * @return false
     */
    static bool
    ValidateWifiAccessPointEnableRequest(const Microsoft::Net::Remote::Wifi::WifiAccessPointEnableRequest* request, Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus& status);

    /**
     * @brief Validate the basic input parameters for the WifiAccessPointSetFrequencyBands reqest.
     *
     * @param request The request to validate.
     * @param result The result to populate with failure information.
     * @return true
     * @return false
     */
    static bool
    ValidateWifiSetFrequencyBandsRequest(const Microsoft::Net::Remote::Wifi::WifiAccessPointSetFrequencyBandsRequest* request, Microsoft::Net::Remote::Wifi::WifiAccessPointSetFrequencyBandsResult* result);

private:
    std::shared_ptr<Microsoft::Net::Wifi::AccessPointManager> m_accessPointManager;
};
} // namespace Microsoft::Net::Remote::Service

#endif // NET_REMOTE_SERVICE_HXX
