
#include <algorithm>
#include <format>
#include <iterator>
#include <optional>
#include <source_location>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <logging/FunctionTracer.hxx>
#include <magic_enum.hpp>
#include <microsoft/net/remote/NetRemoteService.hxx>
#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPointController.hxx>
#include <microsoft/net/wifi/Ieee80211Dot11Adapters.hxx>
#include <plog/Log.h>

using namespace Microsoft::Net::Remote::Service;

using Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus;
using Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatusCode;
using Microsoft::Net::Wifi::AccessPointControllerException;
using Microsoft::Net::Wifi::AccessPointManager;
using Microsoft::Net::Wifi::Dot11AccessPointCapabilities;
using Microsoft::Net::Wifi::IAccessPoint;
using Microsoft::Net::Wifi::IAccessPointController;
using Microsoft::Net::Wifi::Ieee80211AccessPointCapabilities;

NetRemoteService::NetRemoteService(std::shared_ptr<AccessPointManager> accessPointManager) :
    m_accessPointManager(std::move(accessPointManager))
{}

std::shared_ptr<AccessPointManager>
NetRemoteService::GetAccessPointManager() noexcept
{
    return m_accessPointManager;
}

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

using Microsoft::Net::Wifi::Dot11CipherSuite;
using Microsoft::Net::Wifi::Ieee80211CipherSuite;

Microsoft::Net::Wifi::Dot11AccessPointCapabilities
IeeeAccessPointCapabilitiesToNetRemoteAccessPointCapabilities(const Microsoft::Net::Wifi::Ieee80211AccessPointCapabilities& ieeeCapabilities)
{
    using Microsoft::Net::Wifi::ToDot11AkmSuite;
    using Microsoft::Net::Wifi::ToDot11CipherSuite;
    using Microsoft::Net::Wifi::ToDot11FrequencyBand;
    using Microsoft::Net::Wifi::ToDot11PhyType;

    Dot11AccessPointCapabilities capabilities{};

    std::vector<Microsoft::Net::Wifi::Dot11PhyType> phyTypes(std::size(ieeeCapabilities.Protocols));
    std::ranges::transform(ieeeCapabilities.Protocols, std::begin(phyTypes), ToDot11PhyType);

    *capabilities.mutable_phytypes() = {
        std::make_move_iterator(std::begin(phyTypes)),
        std::make_move_iterator(std::end(phyTypes))
    };

    std::vector<Microsoft::Net::Wifi::Dot11FrequencyBand> bands(std::size(ieeeCapabilities.FrequencyBands));
    std::ranges::transform(ieeeCapabilities.FrequencyBands, std::begin(bands), ToDot11FrequencyBand);

    *capabilities.mutable_bands() = {
        std::make_move_iterator(std::begin(bands)),
        std::make_move_iterator(std::end(bands))
    };

    std::vector<Microsoft::Net::Wifi::Dot11AkmSuite> akmSuites(std::size(ieeeCapabilities.AkmSuites));
    std::ranges::transform(ieeeCapabilities.AkmSuites, std::begin(akmSuites), ToDot11AkmSuite);

    *capabilities.mutable_akmsuites() = {
        std::make_move_iterator(std::begin(akmSuites)),
        std::make_move_iterator(std::end(akmSuites))
    };

    std::vector<Microsoft::Net::Wifi::Dot11CipherSuite> cipherSuites(std::size(ieeeCapabilities.CipherSuites));
    std::ranges::transform(ieeeCapabilities.CipherSuites, std::begin(cipherSuites), ToDot11CipherSuite);

    *capabilities.mutable_ciphersuites() = {
        std::make_move_iterator(std::begin(cipherSuites)),
        std::make_move_iterator(std::end(cipherSuites))
    };

    return capabilities;
}

using Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsResultItem;
using Microsoft::Net::Wifi::Dot11AccessPointCapabilities;

static constexpr auto AccessPointIdInvalid{ "invalid" };

Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsResultItem
MakeInvalidAccessPointResultItem()
{
    WifiEnumerateAccessPointsResultItem item{};
    item.set_accesspointid(AccessPointIdInvalid);
    return item;
}

Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsResultItem
IAccessPointToNetRemoteAccessPointResultItem(IAccessPoint& accessPoint)
{
    WifiEnumerateAccessPointsResultItem item{};

    bool isEnabled{ false };
    std::string id{};
    Microsoft::Net::Wifi::Dot11AccessPointCapabilities capabilities{};

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
        capabilities = IeeeAccessPointCapabilitiesToNetRemoteAccessPointCapabilities(capabilitiesIeee80211);
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

Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsResultItem
IAccessPointWeakToNetRemoteAccessPointResultItem(std::weak_ptr<IAccessPoint>& accessPointWeak)
{
    using Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsResultItem;

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
NetRemoteAccessPointResultItemIsInvalid(const Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsResultItem& item)
{
    return (item.accesspointid() == AccessPointIdInvalid);
}

/**
 * @brief Netremote API function tracer.
 */
struct NetRemoteApiTrace :
    public logging::FunctionTracer
{
    /**
     * @brief Construct a new NetRemoteApiTrace object.
     *
     * @param deferEnter Whether to defer the entry log message upon construction.
     * @param location The source code location of the function call.
     */
    NetRemoteApiTrace(bool deferEnter = false, std::source_location location = std::source_location::current()) :
        logging::FunctionTracer(LogTracePrefix, {}, deferEnter, std::move(location))
    {}

private:
    static constexpr auto LogTracePrefix = "[API]";
};

/**
 * @brief Netremote Wi-Fi API function tracer. This should be used for all Wi-Fi API calls. It accepts arguments and
 * return values as optionals and/or pointers to indicate their presence.
 */
struct NetRemoteWifiApiTrace :
    public NetRemoteApiTrace
{
    /**
     * @brief Construct a new Net RemoteWifiApiTrace object.
     *
     * @param accessPointId The access point id associated with the API request, if present.
     * @param operationStatus The result status of the operation, if present.
     * @param location The source code location of the function call.
     */
    NetRemoteWifiApiTrace(std::optional<std::string> accessPointId, const WifiAccessPointOperationStatus* operationStatus, std::source_location location = std::source_location::current()) :
        NetRemoteApiTrace(/* deferEnter= */ true, std::move(location)),
        m_accessPointId(std::move(accessPointId)),
        m_operationStatus(operationStatus)
    {
        if (m_accessPointId.has_value()) {
            AddArgument(AccessPointIdArgName, m_accessPointId.value());
        }

        Enter();
    }

    virtual ~NetRemoteWifiApiTrace()
    {
        if (m_operationStatus != nullptr) {
            AddReturnValue("Status", std::string(magic_enum::enum_name(m_operationStatus->code())));
            if (m_operationStatus->code() != WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded) {
                AddReturnValue("Error Message", m_operationStatus->message());
                SetFailed();
            } else {
                SetSucceeded();
            }
        }
    }

private:
    std::optional<std::string> m_accessPointId;
    const WifiAccessPointOperationStatus* m_operationStatus{ nullptr };

    static constexpr auto AccessPointIdArgName{ "Access Point" };
};
} // namespace detail

::grpc::Status
NetRemoteService::WifiEnumerateAccessPoints([[maybe_unused]] ::grpc::ServerContext* context, [[maybe_unused]] const ::Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsRequest* request, ::Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsResult* response)
{
    using Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsResultItem;

    detail::NetRemoteApiTrace traceMe{};

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

using Microsoft::Net::Wifi::Dot11AuthenticationAlgorithm;
using Microsoft::Net::Wifi::Dot11CipherSuite;
using Microsoft::Net::Wifi::Dot11PhyType;

::grpc::Status
NetRemoteService::WifiAccessPointEnable([[maybe_unused]] ::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiAccessPointEnableRequest* request, ::Microsoft::Net::Remote::Wifi::WifiAccessPointEnableResult* result)
{
    detail::NetRemoteWifiApiTrace traceMe{ request->accesspointid(), result->mutable_status() };

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
NetRemoteService::WifiAccessPointDisable([[maybe_unused]] ::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiAccessPointDisableRequest* request, ::Microsoft::Net::Remote::Wifi::WifiAccessPointDisableResult* result)
{
    detail::NetRemoteWifiApiTrace traceMe{ request->accesspointid(), result->mutable_status() };

    WifiAccessPointOperationStatus status{};
    // TODO: Disable the access point.
    status.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);

    result->set_accesspointid(request->accesspointid());
    *result->mutable_status() = std::move(status);

    return grpc::Status::OK;
}

using Microsoft::Net::Wifi::Dot11PhyType;

::grpc::Status
NetRemoteService::WifiAccessPointSetPhyType([[maybe_unused]] ::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiAccessPointSetPhyTypeRequest* request, ::Microsoft::Net::Remote::Wifi::WifiAccessPointSetPhyTypeResult* result)
{
    using Microsoft::Net::Wifi::Ieee80211AccessPointCapabilities;

    detail::NetRemoteWifiApiTrace traceMe{ request->accesspointid(), result->mutable_status() };

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

using Microsoft::Net::Remote::Wifi::WifiAccessPointSetFrequencyBandsRequest;
using Microsoft::Net::Remote::Wifi::WifiAccessPointSetFrequencyBandsResult;
using Microsoft::Net::Wifi::Dot11FrequencyBand;

namespace detail
{
std::vector<Dot11FrequencyBand>
GetFrequencyBands(const WifiAccessPointSetFrequencyBandsRequest& request)
{
    const auto& frequencyBands = request.frequencybands();

    std::vector<Dot11FrequencyBand> bands(static_cast<std::size_t>(std::size(frequencyBands)));
    std::ranges::transform(frequencyBands, std::begin(bands), [](const auto& frequencyBand) {
        return static_cast<Dot11FrequencyBand>(frequencyBand);
    });

    return bands;
}

} // namespace detail

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
NetRemoteService::WifiAccessPointSetFrequencyBands([[maybe_unused]] ::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiAccessPointSetFrequencyBandsRequest* request, ::Microsoft::Net::Remote::Wifi::WifiAccessPointSetFrequencyBandsResult* result)
{
    using Microsoft::Net::Wifi::FromDot11FrequencyBand;

    detail::NetRemoteWifiApiTrace traceMe{ request->accesspointid(), result->mutable_status() };

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
    const auto& frequencyBands = detail::GetFrequencyBands(*request);
    std::vector<Microsoft::Net::Wifi::Ieee80211FrequencyBand> ieeeFrequencyBands(static_cast<std::size_t>(std::size(frequencyBands)));
    std::ranges::transform(frequencyBands, std::begin(ieeeFrequencyBands), FromDot11FrequencyBand);

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
NetRemoteService::ValidateWifiAccessPointEnableRequest(const ::Microsoft::Net::Remote::Wifi::WifiAccessPointEnableRequest* request, WifiAccessPointOperationStatus& status)
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
