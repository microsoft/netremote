
#include <algorithm>
#include <format>
#include <iterator>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "NetRemoteApiTrace.hxx"
#include "NetRemoteWifiApiTrace.hxx"
#include <magic_enum.hpp>
#include <microsoft/net/remote/NetRemoteService.hxx>
#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPointController.hxx>
#include <microsoft/net/wifi/Ieee80211Dot11Adapters.hxx>
#include <plog/Log.h>

using namespace Microsoft::Net::Remote::Service;
using namespace Microsoft::Net::Remote::Service::Tracing;
using namespace Microsoft::Net::Remote::Wifi;
using namespace Microsoft::Net::Wifi;

namespace detail
{
/**
 * @brief Handle a failure for a request that referecnes an access point.
 *
 * @tparam RequestT The request type. This must contain an access point id (trait).
 * @tparam ResultT The result type. This must contain an access point id and a status (traits).
 * @param request A reference to the request.
 * @param result A reference to the result.
 * @param code The error code to set in the result message.
 * @param message The error message to set in the result message.
 * @param grpcStatus The status to be returned to the client.
 * @return ::grpc::Status
 */
template <
    typename RequestT,
    typename ResultT>
::grpc::Status
HandleFailure(RequestT& request, ResultT& result, WifiAccessPointOperationStatusCode code, std::string_view message, ::grpc::Status grpcStatus = ::grpc::Status::OK)
{
    LOGE << message;

    // Populate status.
    WifiAccessPointOperationStatus status{};
    status.set_code(code);
    status.set_message(std::string(message));

    // Populate result fields expected on error conditions.
    result->set_accesspointid(request->accesspointid());
    *result->mutable_status() = std::move(status);

    return grpcStatus;
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
        HandleFailure(request, result, WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeAccessPointInvalid, std::format("Access point {} not found", accessPointId));
        return nullptr;
    }

    // Attempt to promote the weak reference to a strong reference.
    auto accessPointWeak{ accessPointOpt.value() };
    auto accessPoint = accessPointWeak.lock();
    if (accessPoint == nullptr) {
        HandleFailure(request, result, WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeAccessPointInvalid, std::format("Access point {} is no longer valid", accessPointId));
        return nullptr;
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
        return nullptr;
    }

    // Create a controller for this access point.
    auto accessPointController = accessPoint->CreateController();
    if (accessPointController == nullptr) {
        HandleFailure(request, result, WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInternalError, std::format("Failed to create controller for access point {}", accessPointId));
        return nullptr;
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
    WifiEnumerateAccessPointsResultItem item{};

    bool isEnabled{ false };
    std::string id{};
    Dot11AccessPointCapabilities capabilities{};

    auto interfaceName = accessPoint.GetInterfaceName();
    id.assign(std::cbegin(interfaceName), std::cend(interfaceName));

    auto accessPointController = accessPoint.CreateController();
    if (accessPointController == nullptr) {
        LOGE << std::format("Failed to create controller for access point {}", interfaceName);
        return MakeInvalidAccessPointResultItem();
    }

    try {
        isEnabled = accessPointController->GetIsEnabled();
    } catch (const AccessPointControllerException& apce) {
        LOGE << std::format("Failed to get enabled state for access point {} ({})", interfaceName, apce.what());
        return MakeInvalidAccessPointResultItem();
    }

    try {
        auto capabilitiesIeee80211 = accessPointController->GetCapabilities();
        capabilities = ToDot11AccessPointCapabilities(capabilitiesIeee80211);
    } catch (const AccessPointControllerException& apce) {
        LOGE << std::format("Failed to get capabilities for access point {} ({})", interfaceName, apce.what());
        return MakeInvalidAccessPointResultItem();
    }

    // Populate the result item.
    item.set_accesspointid(std::move(id));
    item.set_isenabled(isEnabled);
    *item.mutable_capabilities() = std::move(capabilities);

    return item;
}

WifiEnumerateAccessPointsResultItem
IAccessPointWeakToNetRemoteAccessPointResultItem(std::weak_ptr<IAccessPoint>& accessPointWeak)
{
    WifiEnumerateAccessPointsResultItem item{};

    auto accessPoint = accessPointWeak.lock();
    if (accessPoint != nullptr) {
        item = IAccessPointToNetRemoteAccessPointResultItem(*accessPoint.get());
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

NetRemoteService::NetRemoteService(std::shared_ptr<AccessPointManager> accessPointManager) :
    m_accessPointManager(std::move(accessPointManager))
{}

std::shared_ptr<AccessPointManager>
NetRemoteService::GetAccessPointManager() noexcept
{
    return m_accessPointManager;
}

::grpc::Status
NetRemoteService::WifiEnumerateAccessPoints([[maybe_unused]] ::grpc::ServerContext* context, [[maybe_unused]] const WifiEnumerateAccessPointsRequest* request, WifiEnumerateAccessPointsResult* response)
{
    NetRemoteApiTrace traceMe{};

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

::grpc::Status
NetRemoteService::WifiAccessPointEnable([[maybe_unused]] ::grpc::ServerContext* context, const WifiAccessPointEnableRequest* request, WifiAccessPointEnableResult* result)
{
    NetRemoteWifiApiTrace traceMe{ request->accesspointid(), result->mutable_status() };

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

::grpc::Status
NetRemoteService::WifiAccessPointDisable([[maybe_unused]] ::grpc::ServerContext* context, const WifiAccessPointDisableRequest* request, WifiAccessPointDisableResult* result)
{
    NetRemoteWifiApiTrace traceMe{ request->accesspointid(), result->mutable_status() };

    WifiAccessPointOperationStatus status{};
    // TODO: Disable the access point.
    status.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);

    result->set_accesspointid(request->accesspointid());
    *result->mutable_status() = std::move(status);

    return grpc::Status::OK;
}

::grpc::Status
NetRemoteService::WifiAccessPointSetPhyType([[maybe_unused]] ::grpc::ServerContext* context, const WifiAccessPointSetPhyTypeRequest* request, WifiAccessPointSetPhyTypeResult* result)
{
    NetRemoteWifiApiTrace traceMe{ request->accesspointid(), result->mutable_status() };

    WifiAccessPointOperationStatus status{};

    auto handleFailure = [&](WifiAccessPointOperationStatusCode statusCode, std::string statusMessage) {
        LOGE << statusMessage;

        status.set_code(statusCode);
        status.set_message(statusMessage);

        result->set_accesspointid(request->accesspointid());
        *result->mutable_status() = std::move(status);

        return grpc::Status::OK;
    };

    // Check if PHY type is provided.
    if (request->phytype() == Dot11PhyType::Dot11PhyTypeUnknown) {
        return handleFailure(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter, "No PHY type provided");
    }

    // Create an AP controller for the requested AP.
    auto accessPointController = detail::TryGetAccessPointController(request, result, m_accessPointManager);
    if (accessPointController == nullptr) {
        return grpc::Status::OK;
    }

    // Convert PHY type to Ieee80211 protocol.
    auto ieeeProtocol = FromDot11PhyType(request->phytype());

    // Check if Ieee80211 protocol is supported by AP.
    try {
        auto accessPointCapabilities = accessPointController->GetCapabilities();
        const auto& supportedIeeeProtocols = accessPointCapabilities.Protocols;
        if (std::ranges::find(supportedIeeeProtocols, ieeeProtocol) == std::cend(supportedIeeeProtocols)) {
            return handleFailure(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeOperationNotSupported, std::format("PHY type not supported by access point {}", request->accesspointid()));
        }
    } catch (const AccessPointControllerException& apce) {
        LOGE << std::format("Failed to get capabilities for access point {} ({})", request->accesspointid(), apce.what());
        return handleFailure(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInternalError, std::format("Failed to get capabilities for access point {}", request->accesspointid()));
    }

    // Set the Ieee80211 protocol.
    try {
        if (!accessPointController->SetProtocol(ieeeProtocol)) {
            return handleFailure(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeOperationNotSupported, std::format("Failed to set PHY type for access point {}", request->accesspointid()));
        }
    } catch (const AccessPointControllerException& apce) {
        return handleFailure(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInternalError, std::format("Failed to set PHY type for access point {} ({})", request->accesspointid(), apce.what()));
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
        detail::HandleFailure(request, result, WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter, "No frequency band provided");
        return false;
    } else if (std::ranges::find(frequencyBands, Dot11FrequencyBand::Dot11FrequencyBandUnknown) != std::cend(frequencyBands)) {
        detail::HandleFailure(request, result, WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter, "Invalid frequency band provided");
        return false;
    }

    return true;
}

::grpc::Status
NetRemoteService::WifiAccessPointSetFrequencyBands([[maybe_unused]] ::grpc::ServerContext* context, const WifiAccessPointSetFrequencyBandsRequest* request, WifiAccessPointSetFrequencyBandsResult* result)
{
    NetRemoteWifiApiTrace traceMe{ request->accesspointid(), result->mutable_status() };

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
        return detail::HandleFailure(request, result, WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInternalError, std::format("Failed to get capabilities for access point {} ({})", request->accesspointid(), apce.what()));
    }

    // Check if requested bands are supported by the AP.
    for (const auto& requestedFrequencyBand : ieeeFrequencyBands) {
        if (std::ranges::find(accessPointCapabilities.FrequencyBands, requestedFrequencyBand) == std::cend(accessPointCapabilities.FrequencyBands)) {
            return detail::HandleFailure(request, result, WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeOperationNotSupported, std::format("Frequency band {} not supported by access point {}", magic_enum::enum_name(requestedFrequencyBand), request->accesspointid()));
        }
    }

    // Attempt to set the frequency bands.
    try {
        const auto setBandsSucceeded = accessPointController->SetFrequencyBands(std::move(ieeeFrequencyBands));
        if (!setBandsSucceeded) {
            return detail::HandleFailure(request, result, WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInternalError, std::format("Failed to set frequency bands for access point {}", request->accesspointid()));
        }
    } catch (const AccessPointControllerException& apce) {
        return detail::HandleFailure(request, result, WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInternalError, std::format("Failed to set frequency bands for access point {} ({})", request->accesspointid(), apce.what()));
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
