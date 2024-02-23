
#include <algorithm>
#include <format>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "NetRemoteApiTrace.hxx"
#include "NetRemoteWifiApiTrace.hxx"
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
#include <microsoft/net/wifi/Ieee80211AccessPointCapabilities.hxx>
#include <microsoft/net/wifi/Ieee80211Dot11Adapters.hxx>
#include <plog/Log.h>

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

    Dot11AccessPointCapabilities dot11AccessPointCapabilities{};
    try {
        auto ieee80211AccessPointCapabilities = accessPointController->GetCapabilities();
        dot11AccessPointCapabilities = ToDot11AccessPointCapabilities(ieee80211AccessPointCapabilities);
    } catch (const AccessPointControllerException& apce) {
        LOGE << std::format("Failed to get capabilities for access point {} ({})", interfaceName, apce.what());
        return MakeInvalidAccessPointResultItem();
    }

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

    WifiAccessPointOperationStatus status{};

    // Validate request is well-formed and has all required parameters.
    if (ValidateWifiAccessPointEnableRequest(request, status)) {
        // TODO: Enable the access point.
        status.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);
    }

    result->set_accesspointid(request->accesspointid());
    *result->mutable_status() = std::move(status);

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

    WifiAccessPointOperationStatus status{};

    // Check if PHY type is provided.
    if (request->phytype() == Dot11PhyType::Dot11PhyTypeUnknown) {
        return HandleFailure(request, result, WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter, "No PHY type provided");
    }

    // Create an AP controller for the requested AP.
    auto accessPointController = detail::TryGetAccessPointController(request, result, m_accessPointManager);
    if (accessPointController == nullptr) {
        return grpc::Status::OK;
    }

    // Convert PHY type to Ieee80211 protocol.
    auto ieee80211Protocol = FromDot11PhyType(request->phytype());

    // Check if Ieee80211 protocol is supported by AP.
    try {
        auto accessPointCapabilities = accessPointController->GetCapabilities();
        const auto& supportedIeee80211Protocols = accessPointCapabilities.Protocols;
        if (std::ranges::find(supportedIeee80211Protocols, ieee80211Protocol) == std::cend(supportedIeee80211Protocols)) {
            return HandleFailure(request, result, WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeOperationNotSupported, std::format("PHY type not supported by access point {}", request->accesspointid()));
        }
    } catch (const AccessPointControllerException& apce) {
        return HandleFailure(request, result, WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInternalError, std::format("Failed to get capabilities for access point {} ({})", request->accesspointid(), apce.what()));
    }

    // Set the Ieee80211 protocol.
    try {
        if (!accessPointController->SetProtocol(ieee80211Protocol)) {
            return HandleFailure(request, result, WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInternalError, std::format("Failed to set PHY type for access point {}", request->accesspointid()));
        }
    } catch (const AccessPointControllerException& apce) {
        return HandleFailure(request, result, WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInternalError, std::format("Failed to set PHY type for access point {} ({})", request->accesspointid(), apce.what()));
    }

    status.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);
    result->set_accesspointid(request->accesspointid());
    *result->mutable_status() = std::move(status);

    return grpc::Status::OK;
}

/* static */
bool
NetRemoteService::ValidateWifiSetFrequencyBandsRequest(const WifiAccessPointSetFrequencyBandsRequest* request, WifiAccessPointSetFrequencyBandsResult* result)
{
    const auto& frequencyBands = request->frequencybands();

    if (std::empty(frequencyBands)) {
        return HandleFailure(request, result, WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter, "No frequency band provided", false);
    }
    if (std::ranges::find(frequencyBands, Dot11FrequencyBand::Dot11FrequencyBandUnknown) != std::cend(frequencyBands)) {
        return HandleFailure(request, result, WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter, "Invalid frequency band provided", false);
    }

    return true;
}

grpc::Status
NetRemoteService::WifiAccessPointSetFrequencyBands([[maybe_unused]] grpc::ServerContext* context, const WifiAccessPointSetFrequencyBandsRequest* request, WifiAccessPointSetFrequencyBandsResult* result)
{
    const NetRemoteWifiApiTrace traceMe{ request->accesspointid(), result->mutable_status() };

    // Validate basic parameters in the request.
    if (!ValidateWifiSetFrequencyBandsRequest(request, result)) {
        return grpc::Status::OK;
    }

    // Create an AP controller for the requested AP.
    auto accessPointController = detail::TryGetAccessPointController(request, result, m_accessPointManager);
    if (accessPointController == nullptr) {
        return grpc::Status::OK;
    }

    // Convert dot11 bands to ieee80211 bands.
    auto ieee80211FrequencyBands = FromDot11SetFrequencyBandsRequest(*request);

    // Obtain capabilities of the access point.
    Ieee80211AccessPointCapabilities accessPointCapabilities{};
    try {
        accessPointCapabilities = accessPointController->GetCapabilities();
    } catch (const AccessPointControllerException& apce) {
        return HandleFailure(request, result, WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInternalError, std::format("Failed to get capabilities for access point {} ({})", request->accesspointid(), apce.what()));
    }

    // Check if requested bands are supported by the AP.
    for (const auto& requestedFrequencyBand : ieee80211FrequencyBands) {
        if (std::ranges::find(accessPointCapabilities.FrequencyBands, requestedFrequencyBand) == std::cend(accessPointCapabilities.FrequencyBands)) {
            return HandleFailure(request, result, WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeOperationNotSupported, std::format("Frequency band {} not supported by access point {}", magic_enum::enum_name(requestedFrequencyBand), request->accesspointid()));
        }
    }

    // Attempt to set the frequency bands.
    try {
        const auto setBandsSucceeded = accessPointController->SetFrequencyBands(std::move(ieee80211FrequencyBands));
        if (!setBandsSucceeded) {
            return HandleFailure(request, result, WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInternalError, std::format("Failed to set frequency bands for access point {}", request->accesspointid()));
        }
    } catch (const AccessPointControllerException& apce) {
        return HandleFailure(request, result, WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInternalError, std::format("Failed to set frequency bands for access point {} ({})", request->accesspointid(), apce.what()));
    }

    // Prepare result with success indication.
    WifiAccessPointOperationStatus status{};
    status.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);

    result->set_accesspointid(request->accesspointid());
    *result->mutable_status() = std::move(status);

    return grpc::Status::OK;
}

/* static */
bool
NetRemoteService::ValidateWifiAccessPointEnableRequest(const WifiAccessPointEnableRequest* request, WifiAccessPointOperationStatus& status)
{
    // Validate required arguments are present. Detailed argument validation is left to the implementation.

    if (!request->has_configuration()) {
        // Configuration isn't required, so exit early.
        return true;
    }

    // Configuration isn't required, but if it's present, it must be valid.
    const auto& configuration = request->configuration();
    if (!configuration.has_ssid()) {
        status.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
        status.set_message("No SSID provided");
        return false;
    }
    if (configuration.phytype() == Dot11PhyType::Dot11PhyTypeUnknown) {
        status.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
        status.set_message("No PHY type provided");
        return false;
    }
    if (configuration.authenticationalgorithm() == Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmUnknown) {
        status.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
        status.set_message("No authentication algorithm provided");
        return false;
    }
    if (configuration.ciphersuite() == Dot11CipherSuite::Dot11CipherSuiteUnknown) {
        status.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
        status.set_message("No cipher suite provided");
        return false;
    }
    if (std::empty(configuration.bands())) {
        status.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
        status.set_message("No radio bands provided");
        return false;
    }

    return true;
}
