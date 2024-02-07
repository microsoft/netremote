
#include <algorithm>
#include <format>
#include <iterator>
#include <string>
#include <vector>

#include <magic_enum.hpp>
#include <microsoft/net/remote/NetRemoteService.hxx>
#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPointController.hxx>
#include <plog/Log.h>

using namespace Microsoft::Net::Remote::Service;

using Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus;
using Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatusCode;
using Microsoft::Net::Wifi::AccessPointControllerException;
using Microsoft::Net::Wifi::AccessPointManager;
using Microsoft::Net::Wifi::IAccessPoint;
using Microsoft::Net::Wifi::IAccessPointController;

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

using Microsoft::Net::Wifi::Dot11PhyType;
using Microsoft::Net::Wifi::Ieee80211Protocol;

Dot11PhyType
IeeeProtocolToNetRemotePhyType(Ieee80211Protocol ieeeProtocol)
{
    switch (ieeeProtocol) {
    case Ieee80211Protocol::Unknown:
        return Dot11PhyType::Dot11PhyTypeUnknown;
    case Ieee80211Protocol::B:
        return Dot11PhyType::Dot11PhyTypeB;
    case Ieee80211Protocol::G:
        return Dot11PhyType::Dot11PhyTypeG;
    case Ieee80211Protocol::N:
        return Dot11PhyType::Dot11PhyTypeN;
    case Ieee80211Protocol::A:
        return Dot11PhyType::Dot11PhyTypeA;
    case Ieee80211Protocol::AC:
        return Dot11PhyType::Dot11PhyTypeAC;
    case Ieee80211Protocol::AD:
        return Dot11PhyType::Dot11PhyTypeAD;
    case Ieee80211Protocol::AX:
        return Dot11PhyType::Dot11PhyTypeAX;
    case Ieee80211Protocol::BE:
        return Dot11PhyType::Dot11PhyTypeBE;
    }

    return Dot11PhyType::Dot11PhyTypeUnknown;
}

Ieee80211Protocol
NetRemotePhyTypeToIeeeProtocol(Dot11PhyType phyType)
{
    switch (phyType) {
    case Dot11PhyType::Dot11PhyTypeB:
        return Ieee80211Protocol::B;
    case Dot11PhyType::Dot11PhyTypeG:
        return Ieee80211Protocol::G;
    case Dot11PhyType::Dot11PhyTypeN:
        return Ieee80211Protocol::N;
    case Dot11PhyType::Dot11PhyTypeA:
        return Ieee80211Protocol::A;
    case Dot11PhyType::Dot11PhyTypeAC:
        return Ieee80211Protocol::AC;
    case Dot11PhyType::Dot11PhyTypeAD:
        return Ieee80211Protocol::AD;
    case Dot11PhyType::Dot11PhyTypeAX:
        return Ieee80211Protocol::AX;
    case Dot11PhyType::Dot11PhyTypeBE:
        return Ieee80211Protocol::BE;
    default:
        return Ieee80211Protocol::Unknown;
    }
}

using Microsoft::Net::Wifi::Dot11FrequencyBand;
using Microsoft::Net::Wifi::Ieee80211FrequencyBand;

Dot11FrequencyBand
IeeeDot11FrequencyBandToNetRemoteDot11FrequencyBand(Ieee80211FrequencyBand ieeeDot11FrequencyBand)
{
    switch (ieeeDot11FrequencyBand) {
    case Ieee80211FrequencyBand::TwoPointFourGHz:
        return Dot11FrequencyBand::Dot11FrequencyBandTwoPoint4GHz;
    case Ieee80211FrequencyBand::FiveGHz:
        return Dot11FrequencyBand::Dot11FrequencyBandFiveGHz;
    case Ieee80211FrequencyBand::SixGHz:
        return Dot11FrequencyBand::Dot11FrequencyBandSixGHz;
    default:
        return Dot11FrequencyBand::Dot11FrequencyBandUnknown;
    }
}

Ieee80211FrequencyBand
NetRemoteDot11FrequencyBandToIeee80211FrequencyBand(Dot11FrequencyBand dot11FrequencyBand)
{
    switch (dot11FrequencyBand) {
    case Dot11FrequencyBand::Dot11FrequencyBand2_4GHz:
        return Ieee80211FrequencyBand::TwoPointFourGHz;
    case Dot11FrequencyBand::Dot11FrequencyBand5_0GHz:
        return Ieee80211FrequencyBand::FiveGHz;
    case Dot11FrequencyBand::Dot11FrequencyBand6_0GHz:
        return Ieee80211FrequencyBand::SixGHz;
    case Dot11FrequencyBand::Dot11FrequencyBandUnknown:
    default:
        return Ieee80211FrequencyBand::Unknown;
    }
}

using Microsoft::Net::Wifi::Dot11AuthenticationAlgorithm;
using Microsoft::Net::Wifi::Ieee80211AuthenticationAlgorithm;

Dot11AuthenticationAlgorithm
IeeeAuthenticationAlgorithmToNetRemoteAuthenticationAlgorithm(Ieee80211AuthenticationAlgorithm ieeeAuthenticationAlgorithm)
{
    switch (ieeeAuthenticationAlgorithm) {
    case Ieee80211AuthenticationAlgorithm::Unknown:
        return Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmUnknown;
    case Ieee80211AuthenticationAlgorithm::OpenSystem:
        return Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmOpenSystem;
    case Ieee80211AuthenticationAlgorithm::SharedKey:
        return Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmSharedKey;
    case Ieee80211AuthenticationAlgorithm::FastBssTransition:
        return Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmFastBssTransition;
    case Ieee80211AuthenticationAlgorithm::Sae:
        return Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmSae;
    case Ieee80211AuthenticationAlgorithm::Fils:
        return Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmFils;
    case Ieee80211AuthenticationAlgorithm::FilsPfs:
        return Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmFilsPfs;
    case Ieee80211AuthenticationAlgorithm::FilsPublicKey:
        return Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmFilsPublicKey;
    case Ieee80211AuthenticationAlgorithm::VendorSpecific:
        return Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmVendorSpecific;
    }

    return Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmUnknown;
}

using Microsoft::Net::Wifi::Dot11AkmSuite;
using Microsoft::Net::Wifi::Ieee80211AkmSuite;

Dot11AkmSuite
Ieee80211AkmSuiteToNetRemoteAkm(Ieee80211AkmSuite akmSuite)
{
    switch (akmSuite) {
    case Ieee80211AkmSuite::Reserved0:
        return Dot11AkmSuite::Dot11AkmSuiteReserved0;
    case Ieee80211AkmSuite::Ieee8021x:
        return Dot11AkmSuite::Dot11AkmSuite8021x;
    case Ieee80211AkmSuite::Psk:
        return Dot11AkmSuite::Dot11AkmSuitePsk;
    case Ieee80211AkmSuite::Ft8021x:
        return Dot11AkmSuite::Dot11AkmSuiteFt8021x;
    case Ieee80211AkmSuite::FtPsk:
        return Dot11AkmSuite::Dot11AkmSuiteFtPsk;
    case Ieee80211AkmSuite::Ieee8021xSha256:
        return Dot11AkmSuite::Dot11AkmSuite8021xSha256;
    case Ieee80211AkmSuite::PskSha256:
        return Dot11AkmSuite::Dot11AkmSuitePskSha256;
    case Ieee80211AkmSuite::Tdls:
        return Dot11AkmSuite::Dot11AkmSuiteTdls;
    case Ieee80211AkmSuite::Sae:
        return Dot11AkmSuite::Dot11AkmSuiteSae;
    case Ieee80211AkmSuite::FtSae:
        return Dot11AkmSuite::Dot11AkmSuiteFtSae;
    case Ieee80211AkmSuite::ApPeerKey:
        return Dot11AkmSuite::Dot11AkmSuiteApPeerKey;
    case Ieee80211AkmSuite::Ieee8021xSuiteB:
        return Dot11AkmSuite::Dot11AkmSuite8021xSuiteB;
    case Ieee80211AkmSuite::Ieee8011xSuiteB192:
        return Dot11AkmSuite::Dot11AkmSuite8021xSuiteB192;
    case Ieee80211AkmSuite::Ft8021xSha384:
        return Dot11AkmSuite::Dot11AkmSuiteFt8021xSha384;
    case Ieee80211AkmSuite::FilsSha256:
        return Dot11AkmSuite::Dot11AkmSuiteFilsSha256;
    case Ieee80211AkmSuite::FilsSha384:
        return Dot11AkmSuite::Dot11AkmSuiteFilsSha384;
    case Ieee80211AkmSuite::FtFilsSha256:
        return Dot11AkmSuite::Dot11AkmSuiteFtFilsSha256;
    case Ieee80211AkmSuite::FtFilsSha384:
        return Dot11AkmSuite::Dot11AkmSuiteFtFilsSha384;
    case Ieee80211AkmSuite::Owe:
        return Dot11AkmSuite::Dot11AkmSuiteOwe;
    case Ieee80211AkmSuite::FtPskSha384:
        return Dot11AkmSuite::Dot11AkmSuiteFtPskSha384;
    case Ieee80211AkmSuite::PskSha384:
        return Dot11AkmSuite::Dot11AkmSuitePskSha384;
    }

    return Dot11AkmSuite::Dot11AkmSuiteUnknown;
}

using Microsoft::Net::Wifi::Dot11CipherSuite;
using Microsoft::Net::Wifi::Ieee80211CipherSuite;

Dot11CipherSuite
IeeeCipherAlgorithmToNetRemoteCipherSuite(Ieee80211CipherSuite ieeeCipherSuite)
{
    switch (ieeeCipherSuite) {
    case Ieee80211CipherSuite::Unknown:
        return Dot11CipherSuite::Dot11CipherSuiteUnknown;
    case Ieee80211CipherSuite::BipCmac128:
        return Dot11CipherSuite::Dot11CipherSuiteBipCmac128;
    case Ieee80211CipherSuite::BipCmac256:
        return Dot11CipherSuite::Dot11CipherSuiteBipCmac256;
    case Ieee80211CipherSuite::BipGmac128:
        return Dot11CipherSuite::Dot11CipherSuiteBipGmac128;
    case Ieee80211CipherSuite::BipGmac256:
        return Dot11CipherSuite::Dot11CipherSuiteBipGmac256;
    case Ieee80211CipherSuite::Ccmp128:
        return Dot11CipherSuite::Dot11CipherSuiteCcmp128;
    case Ieee80211CipherSuite::Ccmp256:
        return Dot11CipherSuite::Dot11CipherSuiteCcmp256;
    case Ieee80211CipherSuite::Gcmp128:
        return Dot11CipherSuite::Dot11CipherSuiteGcmp128;
    case Ieee80211CipherSuite::Gcmp256:
        return Dot11CipherSuite::Dot11CipherSuiteGcmp256;
    case Ieee80211CipherSuite::GroupAddressesTrafficNotAllowed:
        return Dot11CipherSuite::Dot11CipherSuiteGroupAddressesTrafficNotAllowed;
    case Ieee80211CipherSuite::Tkip:
        return Dot11CipherSuite::Dot11CipherSuiteTkip;
    case Ieee80211CipherSuite::UseGroup:
        return Dot11CipherSuite::Dot11CipherSuiteUseGroup;
    case Ieee80211CipherSuite::Wep104:
        return Dot11CipherSuite::Dot11CipherSuiteWep104;
    case Ieee80211CipherSuite::Wep40:
        return Dot11CipherSuite::Dot11CipherSuiteWep40;
    }

    return Dot11CipherSuite::Dot11CipherSuiteUnknown;
}

Microsoft::Net::Wifi::Dot11AccessPointCapabilities
IeeeAccessPointCapabilitiesToNetRemoteAccessPointCapabilities(const Microsoft::Net::Wifi::Ieee80211AccessPointCapabilities& ieeeCapabilities)
{
    using Microsoft::Net::Wifi::Dot11AccessPointCapabilities;
    using Microsoft::Net::Wifi::Ieee80211AccessPointCapabilities;

    Dot11AccessPointCapabilities capabilities{};

    std::vector<Microsoft::Net::Wifi::Dot11PhyType> phyTypes(std::size(ieeeCapabilities.Protocols));
    std::ranges::transform(ieeeCapabilities.Protocols, std::begin(phyTypes), IeeeProtocolToNetRemotePhyType);

    *capabilities.mutable_phytypes() = {
        std::make_move_iterator(std::begin(phyTypes)),
        std::make_move_iterator(std::end(phyTypes))
    };

    std::vector<Microsoft::Net::Wifi::Dot11FrequencyBand> bands(std::size(ieeeCapabilities.FrequencyBands));
    std::ranges::transform(ieeeCapabilities.FrequencyBands, std::begin(bands), IeeeDot11FrequencyBandToNetRemoteDot11FrequencyBand);

    *capabilities.mutable_bands() = {
        std::make_move_iterator(std::begin(bands)),
        std::make_move_iterator(std::end(bands))
    };

    std::vector<Microsoft::Net::Wifi::Dot11AkmSuite> akmSuites(std::size(ieeeCapabilities.AkmSuites));
    std::ranges::transform(ieeeCapabilities.AkmSuites, std::begin(akmSuites), Ieee80211AkmSuiteToNetRemoteAkm);

    *capabilities.mutable_akmsuites() = {
        std::make_move_iterator(std::begin(akmSuites)),
        std::make_move_iterator(std::end(akmSuites))
    };

    std::vector<Microsoft::Net::Wifi::Dot11CipherSuite> cipherSuites(std::size(ieeeCapabilities.CipherSuites));
    std::ranges::transform(ieeeCapabilities.CipherSuites, std::begin(cipherSuites), IeeeCipherAlgorithmToNetRemoteCipherSuite);

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
} // namespace detail

::grpc::Status
NetRemoteService::WifiEnumerateAccessPoints([[maybe_unused]] ::grpc::ServerContext* context, [[maybe_unused]] const ::Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsRequest* request, ::Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsResult* response)
{
    using Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsResultItem;

    LOGD << std::format("Received WifiEnumerateAccessPoints request");

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
NetRemoteService::WifiAccessPointEnable([[maybe_unused]] ::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiAccessPointEnableRequest* request, ::Microsoft::Net::Remote::Wifi::WifiAccessPointEnableResult* response)
{
    LOGD << std::format("Received WifiAccessPointEnable request for access point id {}", request->accesspointid());

    WifiAccessPointOperationStatus status{};

    // Validate request is well-formed and has all required parameters.
    if (ValidateWifiAccessPointEnableRequest(request, status)) {
        // TODO: Enable the access point.
        status.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);
    }

    response->set_accesspointid(request->accesspointid());
    *response->mutable_status() = std::move(status);

    return grpc::Status::OK;
}

::grpc::Status
NetRemoteService::WifiAccessPointDisable([[maybe_unused]] ::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiAccessPointDisableRequest* request, ::Microsoft::Net::Remote::Wifi::WifiAccessPointDisableResult* response)
{
    LOGD << std::format("Received WifiAccessPointDisable request for access point id {}", request->accesspointid());

    WifiAccessPointOperationStatus status{};
    // TODO: Disable the access point.
    status.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);

    response->set_accesspointid(request->accesspointid());
    *response->mutable_status() = std::move(status);

    return grpc::Status::OK;
}

using Microsoft::Net::Wifi::Dot11PhyType;

::grpc::Status
NetRemoteService::WifiAccessPointSetPhyType([[maybe_unused]] ::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiAccessPointSetPhyTypeRequest* request, ::Microsoft::Net::Remote::Wifi::WifiAccessPointSetPhyTypeResult* response)
{
    using Microsoft::Net::Wifi::Ieee80211AccessPointCapabilities;

    LOGD << std::format("Received WifiAccessPointSetPhyType request for access point id {}", request->accesspointid());

    WifiAccessPointOperationStatus status{};

    auto handleFailure = [&](WifiAccessPointOperationStatusCode statusCode, std::string statusMessage) {
        LOGE << statusMessage;

        status.set_code(statusCode);
        status.set_message(statusMessage);

        response->set_accesspointid(request->accesspointid());
        *response->mutable_status() = std::move(status);

        return grpc::Status::OK;
    };

    // Check if PHY type is provided.
    if (request->phytype() == Dot11PhyType::Dot11PhyTypeUnknown) {
        return handleFailure(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter, "No PHY type provided");
    }

    // Create an AP controller for the requested AP.
    auto accessPointController = detail::TryGetAccessPointController(request, response, m_accessPointManager);
    if (accessPointController == nullptr) {
        return grpc::Status::OK;
    }

    // Convert PHY type to Ieee80211 protocol.
    auto ieeeProtocol = detail::NetRemotePhyTypeToIeeeProtocol(request->phytype());

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
    response->set_accesspointid(request->accesspointid());
    *response->mutable_status() = std::move(status);

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

using Microsoft::Net::Wifi::Ieee80211AccessPointCapabilities;

::grpc::Status
NetRemoteService::WifiAccessPointSetFrequencyBands([[maybe_unused]] ::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiAccessPointSetFrequencyBandsRequest* request, ::Microsoft::Net::Remote::Wifi::WifiAccessPointSetFrequencyBandsResult* result)
{
    LOGD << std::format("Received WifiAccessPointSetFrequencyBands request for access point id {}", request->accesspointid());

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
    std::ranges::transform(frequencyBands, std::begin(ieeeFrequencyBands), detail::NetRemoteDot11FrequencyBandToIeee80211FrequencyBand);

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
