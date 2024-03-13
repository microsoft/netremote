
#include <algorithm>
#include <cstddef>
#include <format>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <grpcpp/impl/codegen/status.h>
#include <grpcpp/server_context.h>
#include <magic_enum.hpp>
#include <microsoft/net/remote/NetRemoteService.hxx>
#include <microsoft/net/remote/protocol/NetRemoteWifi.pb.h>
#include <microsoft/net/remote/protocol/WifiCore.pb.h>
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

using namespace Microsoft::Net::Remote::Service;
using namespace Microsoft::Net::Remote::Service::Tracing;
using namespace Microsoft::Net::Remote::Wifi;
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
 * @return WifiEnumerateAccessPointsResultItem
 */
WifiEnumerateAccessPointsResultItem
MakeInvalidAccessPointResultItem()
{
    WifiEnumerateAccessPointsResultItem item{};
    item.set_accesspointid(AccessPointIdInvalid);
    return item;
}

WifiEnumerateAccessPointsResultItem
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
    WifiEnumerateAccessPointsResultItem item{};
    item.set_accesspointid(std::move(id));
    item.set_isenabled(isEnabled);
    *item.mutable_capabilities() = std::move(dot11AccessPointCapabilities);

    return item;
}

WifiEnumerateAccessPointsResultItem
IAccessPointWeakToNetRemoteAccessPointResultItem(std::weak_ptr<IAccessPoint>& accessPointWeak)
{
    WifiEnumerateAccessPointsResultItem item{};

    auto accessPoint = accessPointWeak.lock();
    if (accessPoint != nullptr) {
        item = IAccessPointToNetRemoteAccessPointResultItem(*accessPoint);
    } else {
        item = detail::MakeInvalidAccessPointResultItem();
    }

    return item;
}

bool
NetRemoteAccessPointResultItemIsInvalid(const WifiEnumerateAccessPointsResultItem& item)
{
    return (item.accesspointid() == AccessPointIdInvalid);
}

} // namespace detail

using detail::HandleFailure;

NetRemoteService::NetRemoteService(std::shared_ptr<AccessPointManager> accessPointManager) :
    m_accessPointManager(std::move(accessPointManager))
{}

std::shared_ptr<AccessPointManager>
NetRemoteService::GetAccessPointManager() noexcept
{
    return m_accessPointManager;
}

grpc::Status
NetRemoteService::WifiEnumerateAccessPoints([[maybe_unused]] grpc::ServerContext* context, [[maybe_unused]] const WifiEnumerateAccessPointsRequest* request, WifiEnumerateAccessPointsResult* response)
{
    const NetRemoteApiTrace traceMe{};

    // List all known access points.
    auto accessPoints = m_accessPointManager->GetAllAccessPoints();
    std::vector<WifiEnumerateAccessPointsResultItem> accessPointResultItems(std::size(accessPoints));
    std::ranges::transform(accessPoints, std::begin(accessPointResultItems), [](auto& accessPointWeak) {
        return detail::IAccessPointWeakToNetRemoteAccessPointResultItem(accessPointWeak);
    });

    // Remove any invalid items.
    accessPointResultItems.erase(std::begin(std::ranges::remove_if(accessPointResultItems, detail::NetRemoteAccessPointResultItemIsInvalid)), std::end(accessPointResultItems));

    // Update result.
    *response->mutable_accesspoints() = {
        std::make_move_iterator(std::begin(accessPointResultItems)),
        std::make_move_iterator(std::end(accessPointResultItems))
    };

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

    // Create an AP controller for the requested AP.
    auto accessPointController = detail::TryGetAccessPointController(request, result, m_accessPointManager);
    if (accessPointController == nullptr) {
        return grpc::Status::OK;
    }

    // Obtain current operational state.
    AccessPointOperationalState operationalState{};
    auto operationStatus = accessPointController->GetOperationalState(operationalState);
    if (!operationStatus) {
        return HandleFailure(request, result, operationStatus.Code, std::format("Failed to get operational state for access point {}", request->accesspointid()));
    }

    // Disable the access point if it's not already disabled.
    if (operationalState != AccessPointOperationalState::Disabled) {
        // Disable the access point.
        operationStatus = accessPointController->SetOperationalState(AccessPointOperationalState::Disabled);
        if (!operationStatus) {
            return HandleFailure(request, result, operationStatus.Code, std::format("Failed to set operational state to 'disabled' for access point {}", request->accesspointid()));
        }
    } else {
        LOGI << std::format("Access point {} is in 'disabled' operational state", request->accesspointid());
    }

    WifiAccessPointOperationStatus status{};
    status.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);

    result->set_accesspointid(request->accesspointid());
    *result->mutable_status() = std::move(status);

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

        if (dot11AccessPointConfiguration->authenticationalgorithm() != Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmUnknown) {
            // TODO: set authentication algorithm.
        }

        if (dot11AccessPointConfiguration->ciphersuite() != Dot11CipherSuite::Dot11CipherSuiteUnknown) {
            // TODO: set cipher suite.
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

    // Convert PHY type to Ieee80211 protocol.
    auto ieee80211Protocol = FromDot11PhyType(dot11PhyType);

    // Check if Ieee80211 protocol is supported by AP.
    Ieee80211AccessPointCapabilities accessPointCapabilities{};
    operationStatus = accessPointController->GetCapabilities(accessPointCapabilities);
    if (!operationStatus) {
        wifiOperationStatus.set_code(ToDot11AccessPointOperationStatusCode(operationStatus.Code));
        wifiOperationStatus.set_message(std::format("Failed to get capabilities for access point {} - {}", accessPointId, operationStatus.ToString()));
        return wifiOperationStatus;
    }

    const auto& supportedIeee80211Protocols = accessPointCapabilities.Protocols;
    if (!std::ranges::contains(supportedIeee80211Protocols, ieee80211Protocol)) {
        wifiOperationStatus.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeOperationNotSupported);
        wifiOperationStatus.set_message(std::format("PHY type '{}' not supported by access point {}", magic_enum::enum_name(ieee80211Protocol), accessPointId));
        return wifiOperationStatus;
    }

    // Set the Ieee80211 protocol.
    operationStatus = accessPointController->SetProtocol(ieee80211Protocol);
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
