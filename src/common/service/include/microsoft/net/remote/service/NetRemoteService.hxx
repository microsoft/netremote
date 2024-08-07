
#ifndef NET_REMOTE_SERVICE_HXX
#define NET_REMOTE_SERVICE_HXX

#include <memory>
#include <string>
#include <string_view>

#include <google/protobuf/map.h>
#include <grpcpp/server_context.h>
#include <grpcpp/support/status.h>
#include <microsoft/net/NetworkManager.hxx>
#include <microsoft/net/remote/protocol/NetRemoteService.grpc.pb.h>
#include <microsoft/net/remote/protocol/NetRemoteWifi.pb.h>
#include <microsoft/net/remote/protocol/Network8021x.pb.h>
#include <microsoft/net/remote/protocol/NetworkCore.pb.h>
#include <microsoft/net/remote/protocol/WifiCore.pb.h>
#include <microsoft/net/remote/service/NetRemoteService.hxx>
#include <microsoft/net/wifi/AccessPointManager.hxx>
#include <microsoft/net/wifi/AccessPointOperationStatus.hxx>
#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPointController.hxx>

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
     * @brief Construct a new NetRemoteService object with the specified network manager.
     *
     * @param networkManager The network manager to use.
     */
    explicit NetRemoteService(std::shared_ptr<Microsoft::Net::NetworkManager> networkManager) noexcept;

    /**
     * @brief Get the AccessPointManager object for this service.
     *
     * @return std::shared_ptr<Microsoft::Net::Wifi::AccessPointManager>
     */
    std::shared_ptr<Microsoft::Net::Wifi::AccessPointManager>
    GetAccessPointManager() noexcept;

private:
    /**
     * @brief Enumerate the available network interfaces.
     *
     * @param context
     * @param request
     * @param response
     * @return grpc::Status
     */
    grpc::Status
    NetworkInterfacesEnumerate(grpc::ServerContext* context, const Microsoft::Net::Remote::Network::NetworkEnumerateInterfacesRequest* request, Microsoft::Net::Remote::Network::NetworkEnumerateInterfacesResult* response) override;

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
    WifiAccessPointsEnumerate(grpc::ServerContext* context, const Microsoft::Net::Remote::Wifi::WifiAccessPointsEnumerateRequest* request, Microsoft::Net::Remote::Wifi::WifiAccessPointsEnumerateResult* response) override;

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
     * @brief Set the active PHY type of the access point. The access point must be enabled. This will cause
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

    /**
     * @brief Set the SSID of the access point.
     *
     * @param context
     * @param request
     * @param result
     * @return grpc::Status
     */
    grpc::Status
    WifiAccessPointSetSsid(grpc::ServerContext* context, const Microsoft::Net::Remote::Wifi::WifiAccessPointSetSsidRequest* request, Microsoft::Net::Remote::Wifi::WifiAccessPointSetSsidResult* result) override;

    /**
     * @brief Set the network bridge interface the access point interface will be added to.
     *
     * @param context
     * @param request
     * @param result
     * @return grpc::Status
     */
    grpc::Status
    WifiAccessPointSetNetworkBridge(grpc::ServerContext* context, const Microsoft::Net::Remote::Wifi::WifiAccessPointSetNetworkBridgeRequest* request, Microsoft::Net::Remote::Wifi::WifiAccessPointSetNetworkBridgeResult* result) override;

    /**
     * @brief Set the IEEE 802.1x configuration for the access point.
     *
     * @param context
     * @param request
     * @param result
     * @return grpc::Status
     */
    grpc::Status
    WifiAccessPointSetAuthenticationDot1x(grpc::ServerContext* context, const Microsoft::Net::Remote::Wifi::WifiAccessPointSetAuthenticationDot1xRequest* request, Microsoft::Net::Remote::Wifi::WifiAccessPointSetAuthenticationDot1xResult* result) override;

    /**
     * @brief Get the properties of the specified access point.
     *
     * @param context
     * @param request
     * @param result
     * @return ::grpc::Status
     */
    ::grpc::Status
    WifiAccessPointGetAttributes(grpc::ServerContext* context, const Microsoft::Net::Remote::Wifi::WifiAccessPointGetAttributesRequest* request, Microsoft::Net::Remote::Wifi::WifiAccessPointGetAttributesResult* result) override;

protected:
    /**
     * @brief Attempt to obtain an IAccessPoint instance for the specified access point identifier.
     *
     * @param accessPointId The access point identifier.
     * @param accessPoint Output variable to receive the access point instance.
     * @return Microsoft::Net::Wifi::AccessPointOperationStatus
     */
    Microsoft::Net::Wifi::AccessPointOperationStatus
    TryGetAccessPoint(std::string_view accessPointId, std::shared_ptr<Microsoft::Net::Wifi::IAccessPoint>& accessPoint);

    /**
     * @brief Attempt to obtain an IAccessPointController instance for the access point with the specified identifier.
     *
     * @param accessPointId The access point identifier.
     * @param accessPointController Output variable to receive the access point controller instance.
     * @return Microsoft::Net::Wifi::AccessPointOperationStatus
     */
    Microsoft::Net::Wifi::AccessPointOperationStatus
    TryGetAccessPointController(std::string_view accessPointId, std::shared_ptr<Microsoft::Net::Wifi::IAccessPointController>& accessPointController);

    /**
     * @brief Attempt to obtain an IAccessPointController instance for the specified access point.
     *
     * @param accessPoint The access point to obtain a controller for.
     * @param accessPointController Optional configuration to apply to the access point prior to enablement.
     * @return Microsoft::Net::Wifi::AccessPointOperationStatus
     */
    static Microsoft::Net::Wifi::AccessPointOperationStatus
    TryGetAccessPointController(const std::shared_ptr<Microsoft::Net::Wifi::IAccessPoint>& accessPoint, std::shared_ptr<Microsoft::Net::Wifi::IAccessPointController>& accessPointController);

    /**
     * @brief Enable an access point. This brings the access point online, making it available for use by clients.
     *
     * @param accessPointId The access point identifier.
     * @param dot11AccessPointConfiguration (optional) The access point configuration to enforce prior to enablement.
     * @param accessPointController The access point controller for the specified access point (optional).
     * @return Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus
     */
    Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus
    WifiAccessPointEnableImpl(std::string_view accessPointId, const Microsoft::Net::Wifi::Dot11AccessPointConfiguration* dot11AccessPointConfiguration, std::shared_ptr<Microsoft::Net::Wifi::IAccessPointController> accessPointController = nullptr);

    /**
     * @brief Disable an access point. This will take the access point offline, making it unavailable for use by clients.
     *
     * @param accessPointId The access point identifier.
     * @param accessPointController The access point controller for the specified access point (optional).
     * @return Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus
     */
    Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus
    WifiAccessPointDisableImpl(std::string_view accessPointId, std::shared_ptr<Microsoft::Net::Wifi::IAccessPointController> accessPointController = nullptr);

    /**
     * @brief Set the active PHY type of the access point. The access point must be enabled. This will cause
     * the access point to temporarily go offline while the change is being applied.
     *
     * @param accessPointId The access point identifier.
     * @param dot11PhyType The new PHY type to set.
     * @param accessPointController The access point controller for the specified access point (optional).
     * @return Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus
     */
    Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus
    WifiAccessPointSetPhyTypeImpl(std::string_view accessPointId, Microsoft::Net::Wifi::Dot11PhyType dot11PhyType, std::shared_ptr<Microsoft::Net::Wifi::IAccessPointController> accessPointController = nullptr);

    /**
     * @brief Set the active frequency bands of the access point. The access point must be enabled. This will cause the
     * access point to temporarily go offline while the change is being applied.
     *
     * @param accessPointId The access point identifier.
     * @param dot11FrequencyBands The new frequency bands to set.
     * @param accessPointController The access point controller for the specified access point (optional).
     * @return Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus
     */
    Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus
    WifiAccessPointSetFrequencyBandsImpl(std::string_view accessPointId, std::vector<Microsoft::Net::Wifi::Dot11FrequencyBand>& dot11FrequencyBands, std::shared_ptr<Microsoft::Net::Wifi::IAccessPointController> accessPointController = nullptr);

    /**
     * @brief Set the network bridge interface the access point interface will be added to.
     *
     * @param accessPointId The access point identifier.
     * @param networkBridgeId The network bridge interface id.
     * @param accessPointController The access point controller for the specified access point (optional).
     * @return Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus
     */
    Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus
    WifiAccessPointSetNetworkBridgeImpl(std::string_view accessPointId, std::string_view networkBridgeId, std::shared_ptr<Microsoft::Net::Wifi::IAccessPointController> accessPointController = nullptr);

    /**
     * @brief Set the active authentication algorithms of the access point. If the access point is online, this will
     * cause it to temporarily go offline while the change is being applied.
     *
     * @param accessPointId The access point identifier.
     * @param dot11AuthenticationAlgorithms The new authentication algorithms to set.
     * @param accessPointController The access point controller for the specified access point (optional).
     * @return Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus
     */
    Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus
    WifiAccessPointSetAuthenticationAlgorithsmImpl(std::string_view accessPointId, std::vector<Microsoft::Net::Wifi::Dot11AuthenticationAlgorithm>& dot11AuthenticationAlgorithms, std::shared_ptr<Microsoft::Net::Wifi::IAccessPointController> accessPointController = nullptr);

    /**
     * @brief Set the active authentication data of the access point. If the access point is online, this will cause it to
     * temporarily go offline while the change is being applied.
     *
     * @param accessPointId The access point identifier.
     * @param dot11AuthenticationData The new authentication data to set.
     * @param accessPointController The access point controller for the specified access point (optional).
     * @return Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus
     */
    Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus
    WifiAccessPointSetAuthenticationDataImpl(std::string_view accessPointId, const Microsoft::Net::Wifi::Dot11AuthenticationData& dot11AuthenticationData, std::shared_ptr<Microsoft::Net::Wifi::IAccessPointController> accessPointController = nullptr);

    /**
     * @brief Set the active AKM suites of the access point. If the access point is online, this will cause it to
     * temporarily go offline while the change is being applied.
     *
     * @param accessPointId The access point identifier.
     * @param dot11AkmSuites The new AKM suites to set.
     * @param accessPointController The access point controller for the specified access point (optional).
     * @return Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus
     */
    Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus
    WifiAccessPointSetAkmSuitesImpl(std::string_view accessPointId, std::vector<Microsoft::Net::Wifi::Dot11AkmSuite>& dot11AkmSuites, std::shared_ptr<Microsoft::Net::Wifi::IAccessPointController> accessPointController = nullptr);

    /**
     * @brief Set the active cipher suites of the access point. If the access point is online, this will cause it to
     * temporarily go offline while the change is being applied.
     *
     * @param accessPointId The access point identifier.
     * @param dot11PairwiseCipherSuites The new pairwise cipher suites to set.
     * @param accessPointController The access point controller for the specified access point (optional).
     * @return Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus
     */
    Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus
    WifiAccessPointSetPairwiseCipherSuitesImpl(std::string_view accessPointId, std::unordered_map<Microsoft::Net::Wifi::Dot11SecurityProtocol, std::vector<Microsoft::Net::Wifi::Dot11CipherSuite>>& dot11PairwiseCipherSuites, std::shared_ptr<Microsoft::Net::Wifi::IAccessPointController> accessPointController = nullptr);

    /**
     * @brief Set the SSID of the access point.
     *
     * @param accessPointId The access point identifier.
     * @param dot11Ssid The new SSID to set.
     * @param accessPointController The access point controller for the specified access point (optional).
     * @return Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus
     */
    Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus
    WifiAccessPointSetSsidImpl(std::string_view accessPointId, const Microsoft::Net::Wifi::Dot11Ssid& dot11Ssid, std::shared_ptr<Microsoft::Net::Wifi::IAccessPointController> accessPointController = nullptr);

    /**
     * @brief Set the IEEE 802.1x configuration for the access point.
     *
     * @param accessPointId The access point identifier.
     * @param dot11AuthenticationDot1x The new IEEE 802.1x configuration to set.
     * @param accessPointController  The access point controller for the specified access point (optional).
     * @return Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus
     */
    Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus
    WifiAccessPointSetAuthenticationDot1xImpl(std::string_view accessPointId, const Microsoft::Net::Wifi::Dot11AuthenticationDot1x& dot11AuthenticationDot1x, std::shared_ptr<Microsoft::Net::Wifi::IAccessPointController> accessPointController = nullptr);

    /**
     * @brief Get the sttaic attributes of the specified access point.
     *
     * @param accessPointId The access point identifier.
     * @param dot11AccessPointAttributes Output variable to receive the access point attributes.
     * @return Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus
     */
    Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus
    WifiAccessPointGetAttributesImpl(std::string_view accessPointId, Microsoft::Net::Wifi::Dot11AccessPointAttributes& dot11AccessPointAttributes);

private:
    std::shared_ptr<Microsoft::Net::NetworkManager> m_networkManager;
    std::shared_ptr<Microsoft::Net::Wifi::AccessPointManager> m_accessPointManager;
};
} // namespace Microsoft::Net::Remote::Service

#endif // NET_REMOTE_SERVICE_HXX
