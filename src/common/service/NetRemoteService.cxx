
#include <algorithm>
#include <cstddef>
#include <format>
#include <iterator>
#include <memory>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <grpcpp/impl/codegen/status.h>
#include <grpcpp/server_context.h>
#include <magic_enum.hpp>
#include <microsoft/net/ServiceApiNetworkAdapters.hxx>
#include <microsoft/net/remote/protocol/NetRemoteWifi.pb.h>
#include <microsoft/net/remote/protocol/WifiCore.pb.h>
#include <microsoft/net/remote/service/NetRemoteService.hxx>
#include <microsoft/net/wifi/AccessPointManager.hxx>
#include <microsoft/net/wifi/AccessPointOperationStatus.hxx>
#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPointController.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <microsoft/net/wifi/Ieee80211AccessPointCapabilities.hxx>
#include <microsoft/net/wifi/Ieee80211Dot11Adapters.hxx>
#include <plog/Log.h>

#include "NetRemoteApiTrace.hxx"
#include "NetRemoteWifiApiTrace.hxx"

using namespace Microsoft::Net::Remote;
using namespace Microsoft::Net::Remote::Network;
using namespace Microsoft::Net::Remote::Service;
using namespace Microsoft::Net::Remote::Service::Tracing;
using namespace Microsoft::Net::Remote::Wifi;
using namespace Microsoft::Net;
using namespace Microsoft::Net::Wifi;

namespace detail
{
/**
 * @brief Handle a failure for a request that references an access point.
 *
 * @tparam RequestT The request type. This must contain an access point id (trait).
 * @tparam ResultT The result type. This must contain an access point id and a status (traits).
 * @return ReturnT The type of the return value. Defaults to grpc::Status with a value of grpc::OK.
 * @param request A reference to the request.
 * @param result A reference to the result.
 * @param code The error code to set in the result message.
 * @param message The error message to set in the result message.
 * @param returnValue The value to return from the function.
 */
template <
    typename RequestT,
    typename ResultT,
    typename ReturnT = grpc::Status>
ReturnT
HandleFailure(RequestT& request, ResultT& result, WifiAccessPointOperationStatusCode code, std::string_view message, ReturnT returnValue = {})
{
    LOGE << message;

    // Populate status.
    WifiAccessPointOperationStatus status{};
    status.set_code(code);
    status.set_message(std::string(message));

    // Populate result fields expected on error conditions.
    result->set_accesspointid(request->accesspointid());
    *result->mutable_status() = std::move(status);

    return returnValue;
}

/**
 * @brief Wrapper for HandleFailure that converts a AccessPointOperationStatusCode to a WifiAccessPointOperationStatusCode.
 *
 * @tparam RequestT The request type. This must contain an access point id (trait).
 * @tparam ResultT The result type. This must contain an access point id and a status (traits).
 * @return ReturnT The type of the return value. Defaults to grpc::Status with a value of grpc::OK.
 * @param request A reference to the request.
 * @param result A reference to the result.
 * @param code The error code to set in the result message.
 * @param message The error message to set in the result message.
 * @param returnValue The value to return from the function.
 * @return ReturnT
 */
template <
    typename RequestT,
    typename ResultT,
    typename ReturnT = grpc::Status>
ReturnT
HandleFailure(RequestT& request, ResultT& result, AccessPointOperationStatusCode code, std::string_view message, ReturnT returnValue = {})
{
    return HandleFailure(request, result, ToDot11AccessPointOperationStatusCode(code), message, returnValue);
}

/**
 * @brief Wrapper for HandleFailure that converts a AccessPointOperationStatus to a WifiAccessPointOperationStatus and error details message.
 *
 * @tparam RequestT The request type. This must contain an access point id (trait).
 * @tparam ResultT The result type. This must contain an access point id and a status (traits).
 * @return ReturnT The type of the return value. Defaults to grpc::Status with a value of grpc::OK.
 * @param request A reference to the request.
 * @param result A reference to the result.
 * @param operationStatus The AccessPointOperationStatus to derive the failure code and details message from.
 * @param returnValue The value to return from the function.
 * @return ReturnT
 */
template <
    typename RequestT,
    typename ResultT,
    typename ReturnT = grpc::Status>
ReturnT
HandleFailure(RequestT& request, ResultT& result, const AccessPointOperationStatus& operationStatus, ReturnT returnValue = {})
{
    return HandleFailure(request, result, operationStatus.Code, operationStatus.ToString(), returnValue);
}

/**
 * @brief Attempt to obtain an IAccessPoint instance for the access point in the specified request message.
 *
 * @tparam RequestT The request type. This must contain an access point id (trait).
 * @tparam ResultT The result type. This must contain an access point id and a status (traits).
 * @param request A reference to the request.
 * @param result A reference to the result.
 * @param accessPointManager
 * @return std::shared_ptr<IAccessPoint>
 */
template <
    typename RequestT,
    typename ResultT>
std::shared_ptr<IAccessPoint>
TryGetAccessPoint(RequestT& request, ResultT& result, std::shared_ptr<AccessPointManager>& accessPointManager)
{
    const auto& accessPointId = request->accesspointid();

    // Find the requested AP.
    auto accessPointOpt = accessPointManager->GetAccessPoint(accessPointId);
    if (!accessPointOpt.has_value()) {
        return HandleFailure(request, result, WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeAccessPointInvalid, std::format("Access point {} not found", accessPointId), nullptr);
    }

    // Attempt to promote the weak reference to a strong reference.
    auto accessPointWeak{ accessPointOpt.value() };
    auto accessPoint = accessPointWeak.lock();
    if (accessPoint == nullptr) {
        return HandleFailure(request, result, WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeAccessPointInvalid, std::format("Access point {} is no longer valid", accessPointId), nullptr);
    }

    return accessPoint;
}

/**
 * @brief Attempt to obtain an IAccessPointController instance for the access point in the specified request message.
 *
 * @tparam RequestT The request type. This must contain an access point id (trait).
 * @tparam ResultT The result type. This must contain an access point id and a status (traits).
 * @param request A reference to the request.
 * @param result A reference to the result.
 * @param accessPointManager
 * @return std::shared_ptr<IAccessPointController>
 */
template <
    typename RequestT,
    typename ResultT>
std::shared_ptr<IAccessPointController>
TryGetAccessPointController(RequestT& request, ResultT& result, std::shared_ptr<AccessPointManager>& accessPointManager)
{
    const auto& accessPointId = request->accesspointid();

    // Find the requested AP.
    auto accessPoint = TryGetAccessPoint(request, result, accessPointManager);
    if (accessPoint == nullptr) {
        return HandleFailure(request, result, WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeAccessPointInvalid, std::format("Failed to find access point {}", accessPointId), nullptr);
    }

    // Create a controller for this access point.
    auto accessPointController = accessPoint->CreateController();
    if (accessPointController == nullptr) {
        return HandleFailure(request, result, WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInternalError, std::format("Failed to create controller for access point {}", accessPointId), nullptr);
    }

    return accessPointController;
}

static constexpr auto AccessPointIdInvalid{ "invalid" };

/**
 * @brief Create an invalid access point result item.
 *
 * @return WifiAccessPointsEnumerateResultItem
 */
WifiAccessPointsEnumerateResultItem
MakeInvalidAccessPointResultItem()
{
    WifiAccessPointsEnumerateResultItem item{};
    item.set_accesspointid(AccessPointIdInvalid);
    return item;
}

WifiAccessPointsEnumerateResultItem
IAccessPointToNetRemoteAccessPointResultItem(IAccessPoint& accessPoint)
{
    std::string id{};
    auto interfaceName = accessPoint.GetInterfaceName();
    id.assign(std::cbegin(interfaceName), std::cend(interfaceName));

    auto accessPointController = accessPoint.CreateController();
    if (accessPointController == nullptr) {
        LOGE << std::format("Failed to create controller for access point {}", interfaceName);
        return MakeInvalidAccessPointResultItem();
    }

    AccessPointOperationalState operationalState{};
    auto operationStatus = accessPointController->GetOperationalState(operationalState);
    if (!operationStatus) {
        LOGE << std::format("Failed to get operational state for access point {} ({})", interfaceName, magic_enum::enum_name(operationStatus.Code));
        return MakeInvalidAccessPointResultItem();
    }

    // Obtain capabilities of the access point.
    Ieee80211AccessPointCapabilities ieee80211AccessPointCapabilities{};
    operationStatus = accessPointController->GetCapabilities(ieee80211AccessPointCapabilities);
    if (!operationStatus) {
        LOGE << std::format("Failed to get capabilities for access point {}", interfaceName);
        return MakeInvalidAccessPointResultItem();
    }

    Dot11AccessPointCapabilities dot11AccessPointCapabilities{};
    dot11AccessPointCapabilities = ToDot11AccessPointCapabilities(ieee80211AccessPointCapabilities);

    const bool isEnabled{ operationalState == AccessPointOperationalState::Enabled };

    // Populate the result item.
    WifiAccessPointsEnumerateResultItem item{};
    item.set_accesspointid(std::move(id));
    item.set_isenabled(isEnabled);
    *item.mutable_capabilities() = std::move(dot11AccessPointCapabilities);

    return item;
}

WifiAccessPointsEnumerateResultItem
IAccessPointWeakToNetRemoteAccessPointResultItem(std::weak_ptr<IAccessPoint>& accessPointWeak)
{
    WifiAccessPointsEnumerateResultItem item{};

    auto accessPoint = accessPointWeak.lock();
    if (accessPoint != nullptr) {
        item = IAccessPointToNetRemoteAccessPointResultItem(*accessPoint);
    } else {
        item = detail::MakeInvalidAccessPointResultItem();
    }

    return item;
}

bool
NetRemoteAccessPointResultItemIsInvalid(const WifiAccessPointsEnumerateResultItem& item)
{
    return (item.accesspointid() == AccessPointIdInvalid);
}

} // namespace detail

using detail::HandleFailure;

NetRemoteService::NetRemoteService(std::shared_ptr<NetworkManager> networkManager) noexcept :
    m_networkManager(networkManager),
    m_accessPointManager(networkManager->GetAccessPointManager())
{
}

std::shared_ptr<AccessPointManager>
NetRemoteService::GetAccessPointManager() noexcept
{
    return m_accessPointManager;
}

grpc::Status
NetRemoteService::NetworkInterfacesEnumerate([[maybe_unused]] grpc::ServerContext* context, [[maybe_unused]] const NetworkEnumerateInterfacesRequest* request, NetworkEnumerateInterfacesResult* result)
{
    const NetRemoteApiTrace traceMe{};

    auto networkInterfaceInformation = m_networkManager->GetNetworkInterfaceInformation();
    auto networkInterfaces = ToServiceNetworkInterfaces(networkInterfaceInformation);

    *result->mutable_networkinterfaces() = {
        std::make_move_iterator(std::begin(networkInterfaces)),
        std::make_move_iterator(std::end(networkInterfaces)),
    };

    result->set_status(NetworkOperationStatusCode::NetworkOperationStatusCodeSuccess);

    return grpc::Status::OK;
}

grpc::Status
NetRemoteService::WifiAccessPointsEnumerate([[maybe_unused]] grpc::ServerContext* context, [[maybe_unused]] const WifiAccessPointsEnumerateRequest* request, WifiAccessPointsEnumerateResult* response)
{
    const NetRemoteApiTrace traceMe{};

    // List all known access points.
    auto accessPoints = m_accessPointManager->GetAllAccessPoints();

    // Convert neutral types to dot11 API types.
    std::vector<WifiAccessPointsEnumerateResultItem> accessPointResultItems(std::size(accessPoints));
    std::ranges::transform(accessPoints, std::begin(accessPointResultItems), detail::IAccessPointWeakToNetRemoteAccessPointResultItem);

    // Remove any invalid items.
    accessPointResultItems.erase(std::begin(std::ranges::remove_if(accessPointResultItems, detail::NetRemoteAccessPointResultItemIsInvalid)), std::end(accessPointResultItems));

    // Update result.
    *response->mutable_accesspoints() = {
        std::make_move_iterator(std::begin(accessPointResultItems)),
        std::make_move_iterator(std::end(accessPointResultItems))
    };

    response->mutable_status()->set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);

    return grpc::Status::OK;
}

grpc::Status
NetRemoteService::WifiAccessPointEnable([[maybe_unused]] grpc::ServerContext* context, const WifiAccessPointEnableRequest* request, WifiAccessPointEnableResult* result)
{
    const NetRemoteWifiApiTrace traceMe{ request->accesspointid(), result->mutable_status() };

    const auto* dot11AccessPointConfiguration{ request->has_configuration() ? &request->configuration() : nullptr };
    auto wifiOperationStatus = WifiAccessPointEnableImpl(request->accesspointid(), dot11AccessPointConfiguration);
    result->set_accesspointid(request->accesspointid());
    *result->mutable_status() = std::move(wifiOperationStatus);

    return grpc::Status::OK;
}

grpc::Status
NetRemoteService::WifiAccessPointDisable([[maybe_unused]] grpc::ServerContext* context, const WifiAccessPointDisableRequest* request, WifiAccessPointDisableResult* result)
{
    const NetRemoteWifiApiTrace traceMe{ request->accesspointid(), result->mutable_status() };

    auto wifiOperationStatus = WifiAccessPointDisableImpl(request->accesspointid());
    result->set_accesspointid(request->accesspointid());
    *result->mutable_status() = std::move(wifiOperationStatus);

    return grpc::Status::OK;
}

grpc::Status
NetRemoteService::WifiAccessPointSetPhyType([[maybe_unused]] grpc::ServerContext* context, const WifiAccessPointSetPhyTypeRequest* request, WifiAccessPointSetPhyTypeResult* result)
{
    const NetRemoteWifiApiTrace traceMe{ request->accesspointid(), result->mutable_status() };

    auto wifiOperationStatus = WifiAccessPointSetPhyTypeImpl(request->accesspointid(), request->phytype());
    result->set_accesspointid(request->accesspointid());
    *result->mutable_status() = std::move(wifiOperationStatus);

    return grpc::Status::OK;
}

grpc::Status
NetRemoteService::WifiAccessPointSetFrequencyBands([[maybe_unused]] grpc::ServerContext* context, const WifiAccessPointSetFrequencyBandsRequest* request, WifiAccessPointSetFrequencyBandsResult* result)
{
    const NetRemoteWifiApiTrace traceMe{ request->accesspointid(), result->mutable_status() };

    auto dot11FrequencyBands = ToDot11FrequencyBands(*request);
    auto wifiOperationStatus = WifiAccessPointSetFrequencyBandsImpl(request->accesspointid(), dot11FrequencyBands);
    result->set_accesspointid(request->accesspointid());
    *result->mutable_status() = std::move(wifiOperationStatus);

    return grpc::Status::OK;
}

grpc::Status
NetRemoteService::WifiAccessPointSetNetworkBridge([[maybe_unused]] grpc::ServerContext* context, const WifiAccessPointSetNetworkBridgeRequest* request, WifiAccessPointSetNetworkBridgeResult* result)
{
    const NetRemoteWifiApiTrace traceMe{ request->accesspointid(), result->mutable_status() };

    auto wifiOperationStatus = WifiAccessPointSetNetworkBridgeImpl(request->accesspointid(), request->networkbridgeid());
    result->set_accesspointid(request->accesspointid());
    *result->mutable_status() = std::move(wifiOperationStatus);

    return grpc::Status::OK;
}

AccessPointOperationStatus
NetRemoteService::TryGetAccessPoint(std::string_view accessPointId, std::shared_ptr<IAccessPoint>& accessPoint)
{
    AccessPointOperationStatus operationStatus{ accessPointId };

    // Find the requested AP.
    auto accessPointOpt = m_accessPointManager->GetAccessPoint(accessPointId);
    if (!accessPointOpt.has_value()) {
        operationStatus.Code = AccessPointOperationStatusCode::AccessPointInvalid;
        operationStatus.Details = "access point not found";
        return operationStatus;
    }

    // Attempt to promote the weak reference to a strong reference.
    auto accessPointWeak{ accessPointOpt.value() };
    accessPoint = accessPointWeak.lock();
    if (accessPoint == nullptr) {
        operationStatus.Code = AccessPointOperationStatusCode::AccessPointInvalid;
        operationStatus.Details = "access point no longer valid";
        return operationStatus;
    }

    operationStatus.Code = AccessPointOperationStatusCode::Succeeded;

    return operationStatus;
}

/* static */
AccessPointOperationStatus
NetRemoteService::TryGetAccessPointController(const std::shared_ptr<IAccessPoint>& accessPoint, std::shared_ptr<IAccessPointController>& accessPointController)
{
    AccessPointOperationStatus operationStatus{ accessPoint->GetInterfaceName() };

    accessPointController = accessPoint->CreateController();
    if (accessPointController == nullptr) {
        operationStatus.Code = AccessPointOperationStatusCode::InternalError;
        operationStatus.Details = "failed to create access point controller";
        return operationStatus;
    }

    operationStatus.Code = AccessPointOperationStatusCode::Succeeded;

    return operationStatus;
}

AccessPointOperationStatus
NetRemoteService::TryGetAccessPointController(std::string_view accessPointId, std::shared_ptr<IAccessPointController>& accessPointController)
{
    std::shared_ptr<IAccessPoint> accessPoint{};
    auto operationStatus = TryGetAccessPoint(accessPointId, accessPoint);
    if (!operationStatus.Succeeded()) {
        return operationStatus;
    }

    return TryGetAccessPointController(accessPoint, accessPointController);
}

WifiAccessPointOperationStatus
NetRemoteService::WifiAccessPointEnableImpl(std::string_view accessPointId, const Dot11AccessPointConfiguration* dot11AccessPointConfiguration, std::shared_ptr<IAccessPointController> accessPointController)
{
    WifiAccessPointOperationStatus wifiOperationStatus{};
    AccessPointOperationStatus operationStatus{ accessPointId };

    // Create an AP controller for the requested AP if one wasn't specified.
    if (accessPointController == nullptr) {
        operationStatus = TryGetAccessPointController(accessPointId, accessPointController);
        if (!operationStatus.Succeeded() || accessPointController == nullptr) {
            wifiOperationStatus.set_code(ToDot11AccessPointOperationStatusCode(operationStatus.Code));
            wifiOperationStatus.set_message(std::format("Failed to create access point controller for access point {} - {}", accessPointId, operationStatus.ToString()));
            return wifiOperationStatus;
        }
    }

    // Set all configuration items that are present.
    if (dot11AccessPointConfiguration != nullptr) {
        if (dot11AccessPointConfiguration->phytype() != Dot11PhyType::Dot11PhyTypeUnknown) {
            wifiOperationStatus = WifiAccessPointSetPhyTypeImpl(accessPointId, dot11AccessPointConfiguration->phytype(), accessPointController);
            if (wifiOperationStatus.code() != WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded) {
                return wifiOperationStatus;
            }
        }

        if (dot11AccessPointConfiguration->authenticationalgorithms_size() > 0) {
            auto dot11AuthenticationAlgorithms = ToDot11AuthenticationAlgorithms(*dot11AccessPointConfiguration);
            wifiOperationStatus = WifiAccessPointSetAuthenticationAlgorithsmImpl(accessPointId, dot11AuthenticationAlgorithms, accessPointController);
            if (wifiOperationStatus.code() != WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded) {
                return wifiOperationStatus;
            }
        }

        if (dot11AccessPointConfiguration->authenticationdata().has_psk() || dot11AccessPointConfiguration->authenticationdata().has_sae()) {
            wifiOperationStatus = WifiAccessPointSetAuthenticationDataImpl(accessPointId, dot11AccessPointConfiguration->authenticationdata(), accessPointController);
            if (wifiOperationStatus.code() != WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded) {
                return wifiOperationStatus;
            }
        }

        if (dot11AccessPointConfiguration->akmsuites_size() > 0) {
            auto dot11AkmSuites = ToDot11AkmSuites(*dot11AccessPointConfiguration);
            wifiOperationStatus = WifiAccessPointSetAkmSuitesImpl(accessPointId, dot11AkmSuites, accessPointController);
            if (wifiOperationStatus.code() != WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded) {
                return wifiOperationStatus;
            }
        }

        if (dot11AccessPointConfiguration->pairwiseciphersuites_size() > 0) {
            auto dot11PairwiseCipherSuites = ToDot11CipherSuiteConfigurations(dot11AccessPointConfiguration->pairwiseciphersuites());
            wifiOperationStatus = WifiAccessPointSetPairwiseCipherSuitesImpl(accessPointId, dot11PairwiseCipherSuites, accessPointController);
            if (wifiOperationStatus.code() != WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded) {
                return wifiOperationStatus;
            }
        }

        if (dot11AccessPointConfiguration->has_ssid()) {
            const auto& ssid = dot11AccessPointConfiguration->ssid();
            wifiOperationStatus = WifiAccessPointSetSsidImpl(accessPointId, ssid, accessPointController);
            if (wifiOperationStatus.code() != WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded) {
                return wifiOperationStatus;
            }
        }

        if (!std::empty(dot11AccessPointConfiguration->frequencybands())) {
            auto dot11FrequencyBands = ToDot11FrequencyBands(*dot11AccessPointConfiguration);
            wifiOperationStatus = WifiAccessPointSetFrequencyBandsImpl(accessPointId, dot11FrequencyBands, accessPointController);
            if (wifiOperationStatus.code() != WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded) {
                return wifiOperationStatus;
            }
        }
    }

    // Obtain current operational state.
    AccessPointOperationalState operationalState{};
    operationStatus = accessPointController->GetOperationalState(operationalState);
    if (!operationStatus) {
        wifiOperationStatus.set_code(ToDot11AccessPointOperationStatusCode(operationStatus.Code));
        wifiOperationStatus.set_message(std::format("Failed to get operational state for access point {}", accessPointId));
        return wifiOperationStatus;
    }

    // Enable the access point if it's not already enabled.
    if (operationalState != AccessPointOperationalState::Enabled) {
        // Set the operational state to 'enabled' now that any initial configuration has been set.
        operationStatus = accessPointController->SetOperationalState(AccessPointOperationalState::Enabled);
        if (!operationStatus) {
            wifiOperationStatus.set_code(ToDot11AccessPointOperationStatusCode(operationStatus.Code));
            wifiOperationStatus.set_message(std::format("Failed to set operational state to 'enabled' for access point {}", accessPointId));
            return wifiOperationStatus;
        }
    } else {
        LOGI << std::format("Access point {} is already enabled", accessPointId);
    }

    wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);

    return wifiOperationStatus;
}

Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus
NetRemoteService::WifiAccessPointDisableImpl(std::string_view accessPointId, std::shared_ptr<Microsoft::Net::Wifi::IAccessPointController> accessPointController)
{
    WifiAccessPointOperationStatus wifiOperationStatus{};
    AccessPointOperationStatus operationStatus{ accessPointId };

    // Create an AP controller for the requested AP if one wasn't specified.
    if (accessPointController == nullptr) {
        operationStatus = TryGetAccessPointController(accessPointId, accessPointController);
        if (!operationStatus.Succeeded() || accessPointController == nullptr) {
            wifiOperationStatus.set_code(ToDot11AccessPointOperationStatusCode(operationStatus.Code));
            wifiOperationStatus.set_message(std::format("Failed to create access point controller for access point {} - {}", accessPointId, operationStatus.ToString()));
            return wifiOperationStatus;
        }
    }

    // Obtain current operational state.
    AccessPointOperationalState operationalState{};
    operationStatus = accessPointController->GetOperationalState(operationalState);
    if (!operationStatus) {
        wifiOperationStatus.set_code(ToDot11AccessPointOperationStatusCode(operationStatus.Code));
        wifiOperationStatus.set_message(std::format("Failed to get operational state for access point {}", accessPointId));
        return wifiOperationStatus;
    }

    // Disable the access point if it's not already disabled.
    if (operationalState != AccessPointOperationalState::Disabled) {
        // Disable the access point.
        operationStatus = accessPointController->SetOperationalState(AccessPointOperationalState::Disabled);
        if (!operationStatus) {
            wifiOperationStatus.set_code(ToDot11AccessPointOperationStatusCode(operationStatus.Code));
            wifiOperationStatus.set_message(std::format("Failed to set operational state to 'disabled' for access point {}", accessPointId));
            return wifiOperationStatus;
        }
    } else {
        LOGI << std::format("Access point {} is already in 'disabled' operational state", accessPointId);
    }

    wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);

    return wifiOperationStatus;
}

WifiAccessPointOperationStatus
NetRemoteService::WifiAccessPointSetPhyTypeImpl(std::string_view accessPointId, Dot11PhyType dot11PhyType, std::shared_ptr<IAccessPointController> accessPointController)
{
    WifiAccessPointOperationStatus wifiOperationStatus{};

    // Check if PHY type is provided.
    if (dot11PhyType == Dot11PhyType::Dot11PhyTypeUnknown) {
        wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
        wifiOperationStatus.set_message("No PHY type provided");
        return wifiOperationStatus;
    }

    AccessPointOperationStatus operationStatus{ accessPointId };

    // Create an AP controller for the requested AP if one wasn't specified.
    if (accessPointController == nullptr) {
        operationStatus = TryGetAccessPointController(accessPointId, accessPointController);
        if (!operationStatus.Succeeded() || accessPointController == nullptr) {
            wifiOperationStatus.set_code(ToDot11AccessPointOperationStatusCode(operationStatus.Code));
            wifiOperationStatus.set_message(std::format("Failed to create access point controller for access point {} - {}", accessPointId, operationStatus.ToString()));
            return wifiOperationStatus;
        }
    }

    // Convert dot11 PHY type to Ieee80211 PHY type.
    auto ieee80211PhyType = FromDot11PhyType(dot11PhyType);

    // Check if Ieee80211 PHY type is supported by AP.
    Ieee80211AccessPointCapabilities accessPointCapabilities{};
    operationStatus = accessPointController->GetCapabilities(accessPointCapabilities);
    if (!operationStatus) {
        wifiOperationStatus.set_code(ToDot11AccessPointOperationStatusCode(operationStatus.Code));
        wifiOperationStatus.set_message(std::format("Failed to get capabilities for access point {} - {}", accessPointId, operationStatus.ToString()));
        return wifiOperationStatus;
    }

    const auto& supportedIeee80211PhyTypes = accessPointCapabilities.PhyTypes;
    if (!std::ranges::contains(supportedIeee80211PhyTypes, ieee80211PhyType)) {
        wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeOperationNotSupported);
        wifiOperationStatus.set_message(std::format("PHY type '{}' not supported by access point {}", magic_enum::enum_name(ieee80211PhyType), accessPointId));
        return wifiOperationStatus;
    }

    // Set the Ieee80211 PHY type.
    operationStatus = accessPointController->SetPhyType(ieee80211PhyType);
    if (!operationStatus) {
        wifiOperationStatus.set_code(ToDot11AccessPointOperationStatusCode(operationStatus.Code));
        wifiOperationStatus.set_message(std::format("Failed to set PHY type for access point {} - {}", accessPointId, operationStatus.ToString()));
        return wifiOperationStatus;
    }

    wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);

    return wifiOperationStatus;
}

WifiAccessPointOperationStatus
NetRemoteService::WifiAccessPointSetFrequencyBandsImpl(std::string_view accessPointId, std::vector<Dot11FrequencyBand>& dot11FrequencyBands, std::shared_ptr<IAccessPointController> accessPointController)
{
    WifiAccessPointOperationStatus wifiOperationStatus{};

    // Validate basic parameters in the request.
    if (std::empty(dot11FrequencyBands)) {
        wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
        wifiOperationStatus.set_message("No frequency band provided");
        return wifiOperationStatus;
    }
    if (std::ranges::contains(dot11FrequencyBands, Dot11FrequencyBand::Dot11FrequencyBandUnknown)) {
        wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
        wifiOperationStatus.set_message("Invalid frequency band provided");
        return wifiOperationStatus;
    }

    AccessPointOperationStatus operationStatus{ accessPointId };

    // Create an AP controller for the requested AP if one wasn't specified.
    if (accessPointController == nullptr) {
        operationStatus = TryGetAccessPointController(accessPointId, accessPointController);
        if (!operationStatus.Succeeded() || accessPointController == nullptr) {
            wifiOperationStatus.set_code(ToDot11AccessPointOperationStatusCode(operationStatus.Code));
            wifiOperationStatus.set_message(std::format("Failed to create access point controller for access point - {}", accessPointId, operationStatus.ToString()));
            return wifiOperationStatus;
        }
    }

    // Convert dot11 bands to ieee80211 bands.
    std::vector<Ieee80211FrequencyBand> ieee80211FrequencyBands(static_cast<std::size_t>(std::size(dot11FrequencyBands)));
    std::ranges::transform(dot11FrequencyBands, std::begin(ieee80211FrequencyBands), FromDot11FrequencyBand);

    // Obtain capabilities of the access point.
    Ieee80211AccessPointCapabilities accessPointCapabilities{};
    operationStatus = accessPointController->GetCapabilities(accessPointCapabilities);
    if (!operationStatus.Succeeded()) {
        wifiOperationStatus.set_code(ToDot11AccessPointOperationStatusCode(operationStatus.Code));
        wifiOperationStatus.set_message(std::format("Failed to get capabilities for access point {} - {}", accessPointId, operationStatus.ToString()));
        return wifiOperationStatus;
    }

    // Check if requested bands are supported by the AP.
    for (const auto& requestedFrequencyBand : ieee80211FrequencyBands) {
        if (!std::ranges::contains(accessPointCapabilities.FrequencyBands, requestedFrequencyBand)) {
            wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeOperationNotSupported);
            wifiOperationStatus.set_message(std::format("Frequency band '{}' not supported by access point {}", magic_enum::enum_name(requestedFrequencyBand), accessPointId));
            return wifiOperationStatus;
        }
    }

    // Attempt to set the frequency bands.
    operationStatus = accessPointController->SetFrequencyBands(std::move(ieee80211FrequencyBands));
    if (!operationStatus.Succeeded()) {
        wifiOperationStatus.set_code(ToDot11AccessPointOperationStatusCode(operationStatus.Code));
        wifiOperationStatus.set_message(std::format("Failed to set frequency bands for access point {} - {}", accessPointId, operationStatus.ToString()));
        return wifiOperationStatus;
    }

    wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);

    return wifiOperationStatus;
}

WifiAccessPointOperationStatus
NetRemoteService::WifiAccessPointSetNetworkBridgeImpl(std::string_view accessPointId, std::string_view networkBridgeId, std::shared_ptr<Microsoft::Net::Wifi::IAccessPointController> accessPointController)
{
    WifiAccessPointOperationStatus wifiOperationStatus{};
    AccessPointOperationStatus operationStatus{ accessPointId };

    // Create an AP controller for the requested AP if one wasn't specified.
    if (accessPointController == nullptr) {
        operationStatus = TryGetAccessPointController(accessPointId, accessPointController);
        if (!operationStatus.Succeeded() || accessPointController == nullptr) {
            wifiOperationStatus.set_code(ToDot11AccessPointOperationStatusCode(operationStatus.Code));
            wifiOperationStatus.set_message(std::format("Failed to create access point controller for access point - {}", accessPointId, operationStatus.ToString()));
            return wifiOperationStatus;
        }
    }

    // Attempt to set the network bridge.
    operationStatus = accessPointController->SetNetworkBridge(networkBridgeId);
    if (!operationStatus.Succeeded()) {
        wifiOperationStatus.set_code(ToDot11AccessPointOperationStatusCode(operationStatus.Code));
        wifiOperationStatus.set_message(std::format("Failed to set network bridge for access point {} - {}", accessPointId, operationStatus.ToString()));
        return wifiOperationStatus;
    }

    wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);

    return wifiOperationStatus;
}

WifiAccessPointOperationStatus
NetRemoteService::WifiAccessPointSetAuthenticationAlgorithsmImpl(std::string_view accessPointId, std::vector<Dot11AuthenticationAlgorithm>& dot11AuthenticationAlgorithms, std::shared_ptr<IAccessPointController> accessPointController)
{
    WifiAccessPointOperationStatus wifiOperationStatus{};

    // Validate basic parameters in the request.
    if (std::empty(dot11AuthenticationAlgorithms)) {
        wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
        wifiOperationStatus.set_message("No authentication algorithms provided");
        return wifiOperationStatus;
    }
    if (std::ranges::contains(dot11AuthenticationAlgorithms, Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmUnknown)) {
        wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
        wifiOperationStatus.set_message("Invalid authentication algorithm provided");
        return wifiOperationStatus;
    }

    AccessPointOperationStatus operationStatus{ accessPointId };

    // Create an AP controller for the requested AP if one wasn't specified.
    if (accessPointController == nullptr) {
        operationStatus = TryGetAccessPointController(accessPointId, accessPointController);
        if (!operationStatus.Succeeded() || accessPointController == nullptr) {
            wifiOperationStatus.set_code(ToDot11AccessPointOperationStatusCode(operationStatus.Code));
            wifiOperationStatus.set_message(std::format("Failed to create access point controller for access point {} - {}", accessPointId, operationStatus.ToString()));
            return wifiOperationStatus;
        }
    }

    // Convert to 802.11 neutral type.
    std::vector<Ieee80211AuthenticationAlgorithm> ieee80211AuthenticationAlgorithms(static_cast<std::size_t>(std::size(dot11AuthenticationAlgorithms)));
    std::ranges::transform(dot11AuthenticationAlgorithms, std::begin(ieee80211AuthenticationAlgorithms), FromDot11AuthenticationAlgorithm);
    if (std::ranges::contains(ieee80211AuthenticationAlgorithms, Ieee80211AuthenticationAlgorithm::Unknown)) {
        wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
        wifiOperationStatus.set_message("Invalid authentication algorithm provided");
        return wifiOperationStatus;
    }

    // Set the algorithms.
    operationStatus = accessPointController->SetAuthenticationAlgorithms(std::move(ieee80211AuthenticationAlgorithms));
    if (!operationStatus.Succeeded()) {
        wifiOperationStatus.set_code(ToDot11AccessPointOperationStatusCode(operationStatus.Code));
        wifiOperationStatus.set_message(std::format("Failed to set authentication algorithms for access point {} - {}", accessPointId, operationStatus.ToString()));
        return wifiOperationStatus;
    }

    wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);

    return wifiOperationStatus;
}

WifiAccessPointOperationStatus
NetRemoteService::WifiAccessPointSetAuthenticationDataImpl(std::string_view accessPointId, const Dot11AuthenticationData& dot11AuthenticationData, std::shared_ptr<IAccessPointController> accessPointController)
{
    WifiAccessPointOperationStatus wifiOperationStatus{};

    // Validate basic parameters in the request.
    if (!dot11AuthenticationData.has_psk() && !dot11AuthenticationData.has_sae()) {
        wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
        wifiOperationStatus.set_message("No authentication data provided");
        return wifiOperationStatus;
    }

    if (dot11AuthenticationData.has_psk()) {
        const auto& dataPsk = dot11AuthenticationData.psk();
        if (!dataPsk.has_psk()) {
            wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
            wifiOperationStatus.set_message("No PSK data provided");
            return wifiOperationStatus;
        }
        const auto& psk = dataPsk.psk();
        if (!psk.has_passphrase() && !psk.has_value()) {
            wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
            wifiOperationStatus.set_message("No PSK value or passphrase provided");
            return wifiOperationStatus;
        }
        if (psk.has_passphrase()) {
            const auto& pskPassphrase = psk.passphrase();
            if (std::empty(pskPassphrase)) {
                wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
                wifiOperationStatus.set_message("No PSK passphrase provided (empty)");
                return wifiOperationStatus;
            }
            const auto pskPassphraseSize = std::size(pskPassphrase);
            if (pskPassphraseSize < Ieee80211RsnaPskPassphraseLengthMinimum || pskPassphraseSize > Ieee80211RsnaPskPassphraseLengthMaximum) {
                wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
                wifiOperationStatus.set_message(std::format("Invalid PSK passphrase size '{}' provided (expected {} to {} characters)", pskPassphraseSize, Ieee80211RsnaPskPassphraseLengthMinimum, Ieee80211RsnaPskPassphraseLengthMaximum));
                return wifiOperationStatus;
            }
        } else if (psk.has_value()) {
            const auto& pskValue = psk.value();
            if (!pskValue.has_hex() && !pskValue.has_raw()) {
                wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
                wifiOperationStatus.set_message("No PSK value data provided");
                return wifiOperationStatus;
            }
            if (pskValue.has_hex()) {
                const auto& pskHex = pskValue.hex();
                if (std::size(pskHex) != Ieee80211RsnaPskLength * 2) {
                    wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
                    wifiOperationStatus.set_message(std::format("Invalid PSK hex value size '{}' provided (expected {} characters)", std::size(pskHex), Ieee80211RsnaPskLength * 2));
                    return wifiOperationStatus;
                }
            } else if (pskValue.has_raw()) {
                const auto& pskRaw = pskValue.raw();
                if (std::size(pskRaw) != Ieee80211RsnaPskLength) {
                    wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
                    wifiOperationStatus.set_message(std::format("Invalid PSK raw value size '{}' provided (expected {} bytes)", std::size(pskRaw), Ieee80211RsnaPskLength));
                    return wifiOperationStatus;
                }
            }
        }
    }

    if (dot11AuthenticationData.has_sae()) {
        const auto& dataSae = dot11AuthenticationData.sae();
        if (std::empty(dataSae.passwords())) {
            wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
            wifiOperationStatus.set_message("No SAE passwords provided");
            return wifiOperationStatus;
        }
    }

    AccessPointOperationStatus operationStatus{ accessPointId };

    // Create an AP controller for the requested AP if one wasn't specified.
    if (accessPointController == nullptr) {
        operationStatus = TryGetAccessPointController(accessPointId, accessPointController);
        if (!operationStatus.Succeeded() || accessPointController == nullptr) {
            wifiOperationStatus.set_code(ToDot11AccessPointOperationStatusCode(operationStatus.Code));
            wifiOperationStatus.set_message(std::format("Failed to create access point controller for access point {} - {}", accessPointId, operationStatus.ToString()));
            return wifiOperationStatus;
        }
    }

    // Convert to 802.11 neutral type.
    auto ieee80211AuthenticationData = FromDot11AuthenticationData(dot11AuthenticationData);

    // Set the authentication data.
    operationStatus = accessPointController->SetAuthenticationData(std::move(ieee80211AuthenticationData));
    if (!operationStatus.Succeeded()) {
        wifiOperationStatus.set_code(ToDot11AccessPointOperationStatusCode(operationStatus.Code));
        wifiOperationStatus.set_message(std::format("Failed to set authentication data for access point {} - {}", accessPointId, operationStatus.ToString()));
        return wifiOperationStatus;
    }

    wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);

    return wifiOperationStatus;
}

WifiAccessPointOperationStatus
NetRemoteService::WifiAccessPointSetAkmSuitesImpl(std::string_view accessPointId, std::vector<Dot11AkmSuite>& dot11AkmSuites, std::shared_ptr<IAccessPointController> accessPointController)
{
    WifiAccessPointOperationStatus wifiOperationStatus{};

    // Validate basic parameters in the request.
    if (std::empty(dot11AkmSuites)) {
        wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
        wifiOperationStatus.set_message("No akm suites provided");
        return wifiOperationStatus;
    }
    if (std::ranges::contains(dot11AkmSuites, Dot11AkmSuite::Dot11AkmSuiteUnknown)) {
        wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
        wifiOperationStatus.set_message("Invalid akm suite provided");
        return wifiOperationStatus;
    }

    AccessPointOperationStatus operationStatus{ accessPointId };

    // Create an AP controller for the requested AP if one wasn't specified.
    if (accessPointController == nullptr) {
        operationStatus = TryGetAccessPointController(accessPointId, accessPointController);
        if (!operationStatus.Succeeded() || accessPointController == nullptr) {
            wifiOperationStatus.set_code(ToDot11AccessPointOperationStatusCode(operationStatus.Code));
            wifiOperationStatus.set_message(std::format("Failed to create access point controller for access point {} - {}", accessPointId, operationStatus.ToString()));
            return wifiOperationStatus;
        }
    }

    // Convert to 802.11 neutral type.
    std::vector<Ieee80211AkmSuite> ieee80211AkmSuites(static_cast<std::size_t>(std::size(dot11AkmSuites)));
    std::ranges::transform(dot11AkmSuites, std::begin(ieee80211AkmSuites), FromDot11AkmSuite);
    if (std::ranges::contains(ieee80211AkmSuites, Ieee80211AkmSuite::Unknown)) {
        wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
        wifiOperationStatus.set_message("Invalid akm suite provided");
        return wifiOperationStatus;
    }

    // Set the algorithms.
    operationStatus = accessPointController->SetAkmSuites(std::move(ieee80211AkmSuites));
    if (!operationStatus.Succeeded()) {
        wifiOperationStatus.set_code(ToDot11AccessPointOperationStatusCode(operationStatus.Code));
        wifiOperationStatus.set_message(std::format("Failed to set akm suites for access point {} - {}", accessPointId, operationStatus.ToString()));
        return wifiOperationStatus;
    }

    wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);

    return wifiOperationStatus;
}

WifiAccessPointOperationStatus
NetRemoteService::WifiAccessPointSetPairwiseCipherSuitesImpl(std::string_view accessPointId, std::unordered_map<Dot11SecurityProtocol, std::vector<Dot11CipherSuite>>& dot11PairwiseCipherSuites, std::shared_ptr<IAccessPointController> accessPointController)
{
    WifiAccessPointOperationStatus wifiOperationStatus{};

    // Validate basic parameters in the request.
    if (std::empty(dot11PairwiseCipherSuites)) {
        wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
        wifiOperationStatus.set_message("No cipher suites provided");
        return wifiOperationStatus;
    }
    if (std::ranges::contains(dot11PairwiseCipherSuites | std::views::keys, Dot11SecurityProtocol::Dot11SecurityProtocolUnknown)) {
        wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
        wifiOperationStatus.set_message("Invalid security protocol provided");
        return wifiOperationStatus;
    }

    AccessPointOperationStatus operationStatus{ accessPointId };

    // Create an AP controller for the requested AP if one wasn't specified.
    if (accessPointController == nullptr) {
        operationStatus = TryGetAccessPointController(accessPointId, accessPointController);
        if (!operationStatus.Succeeded() || accessPointController == nullptr) {
            wifiOperationStatus.set_code(ToDot11AccessPointOperationStatusCode(operationStatus.Code));
            wifiOperationStatus.set_message(std::format("Failed to create access point controller for access point {} - {}", accessPointId, operationStatus.ToString()));
            return wifiOperationStatus;
        }
    }

    // Convert to 802.11 neutral type.
    auto ieee80211CipherSuites = FromDot11CipherSuiteConfigurations(dot11PairwiseCipherSuites);
    if (std::ranges::contains(ieee80211CipherSuites | std::views::keys, Ieee80211SecurityProtocol::Unknown)) {
        wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
        wifiOperationStatus.set_message("Invalid security protocol provided");
        return wifiOperationStatus;
    }

    // Set the cipher suites.
    operationStatus = accessPointController->SetPairwiseCipherSuites(std::move(ieee80211CipherSuites));
    if (!operationStatus.Succeeded()) {
        wifiOperationStatus.set_code(ToDot11AccessPointOperationStatusCode(operationStatus.Code));
        wifiOperationStatus.set_message(std::format("Failed to set cipher suites for access point {} - {}", accessPointId, operationStatus.ToString()));
        return wifiOperationStatus;
    }

    wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);

    return wifiOperationStatus;
}

WifiAccessPointOperationStatus
NetRemoteService::WifiAccessPointSetSsidImpl(std::string_view accessPointId, const Dot11Ssid& dot11Ssid, std::shared_ptr<IAccessPointController> accessPointController)
{
    WifiAccessPointOperationStatus wifiOperationStatus{};

    if (dot11Ssid.Value_case() == Dot11Ssid::ValueCase::VALUE_NOT_SET) {
        wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
        wifiOperationStatus.set_message("No SSID provided");
        return wifiOperationStatus;
    }

    AccessPointOperationStatus operationStatus{ accessPointId };

    // Create an AP controller for the requested AP if one wasn't specified.
    if (accessPointController == nullptr) {
        operationStatus = TryGetAccessPointController(accessPointId, accessPointController);
        if (!operationStatus.Succeeded() || accessPointController == nullptr) {
            wifiOperationStatus.set_code(ToDot11AccessPointOperationStatusCode(operationStatus.Code));
            wifiOperationStatus.set_message(std::format("Failed to create access point controller for access point {} - {}", accessPointId, operationStatus.ToString()));
            return wifiOperationStatus;
        }
    }

    // Set the SSID.
    switch (dot11Ssid.Value_case()) {
    case Dot11Ssid::ValueCase::kName:
        operationStatus = accessPointController->SetSsid(dot11Ssid.name());
        break;
    case Dot11Ssid::ValueCase::kHex:
        operationStatus = accessPointController->SetSsid(dot11Ssid.hex());
        break;
    case Dot11Ssid::ValueCase::kData:
        operationStatus = accessPointController->SetSsid(dot11Ssid.data());
        break;
    case Dot11Ssid::ValueCase::VALUE_NOT_SET:
        [[fallthrough]];
    default:
        operationStatus.Code = AccessPointOperationStatusCode::InvalidParameter;
        operationStatus.Details = "No SSID provided";
        break;
    }

    if (!operationStatus.Succeeded()) {
        wifiOperationStatus.set_code(ToDot11AccessPointOperationStatusCode(operationStatus.Code));
        wifiOperationStatus.set_message(std::format("Failed to set SSID for access point {} - {}", accessPointId, operationStatus.ToString()));
        return wifiOperationStatus;
    }

    wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);

    return wifiOperationStatus;
}
