
#include <algorithm>
#include <format>
#include <iterator>
#include <string>
#include <vector>

#include <microsoft/net/remote/NetRemoteService.hxx>
#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPointController.hxx>
#include <plog/Log.h>

using namespace Microsoft::Net::Remote::Service;

using Microsoft::Net::Wifi::AccessPointControllerException;
using Microsoft::Net::Wifi::AccessPointManager;
using Microsoft::Net::Wifi::IAccessPoint;

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
    case Ieee80211FrequencyBand::Unknown:
        return Dot11FrequencyBand::Dot11FrequencyBandUnknown;
    case Ieee80211FrequencyBand::TwoPointFourGHz:
        return Dot11FrequencyBand::Dot11FrequencyBandTwoPoint4GHz;
    case Ieee80211FrequencyBand::FiveGHz:
        return Dot11FrequencyBand::Dot11FrequencyBandFiveGHz;
    case Ieee80211FrequencyBand::SixGHz:
        return Dot11FrequencyBand::Dot11FrequencyBandSixGHz;
    }

    return Dot11FrequencyBand::Dot11FrequencyBandUnknown;
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

Ieee80211AuthenticationAlgorithm
NetRemoteAuthenticationAlgorithmToIeeeAuthenticationAlgorithm(Dot11AuthenticationAlgorithm authenticationAlgorithm)
{
    switch (authenticationAlgorithm) {
    case Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmUnknown:
        return Ieee80211AuthenticationAlgorithm::Unknown;
    case Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmOpenSystem:
        return Ieee80211AuthenticationAlgorithm::OpenSystem;
    case Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmSharedKey:
        return Ieee80211AuthenticationAlgorithm::SharedKey;
    case Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmFastBssTransition:
        return Ieee80211AuthenticationAlgorithm::FastBssTransition;
    case Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmSae:
        return Ieee80211AuthenticationAlgorithm::Sae;
    case Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmFils:
        return Ieee80211AuthenticationAlgorithm::Fils;
    case Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmFilsPfs:
        return Ieee80211AuthenticationAlgorithm::FilsPfs;
    case Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmFilsPublicKey:
        return Ieee80211AuthenticationAlgorithm::FilsPublicKey;
    case Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmVendorSpecific:
        return Ieee80211AuthenticationAlgorithm::VendorSpecific;
    default:
        return Ieee80211AuthenticationAlgorithm::Unknown;
    }
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
    case Ieee80211AkmSuite::Ieee8021xSuiteB192:
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

Ieee80211AkmSuite
NetRemoteAkmToIeee80211AkmSuite(Dot11AkmSuite akmSuite)
{
    switch (akmSuite) {
    case Dot11AkmSuite::Dot11AkmSuite8021x:
        return Ieee80211AkmSuite::Ieee8021x;
    case Dot11AkmSuite::Dot11AkmSuitePsk:
        return Ieee80211AkmSuite::Psk;
    case Dot11AkmSuite::Dot11AkmSuiteFt8021x:
        return Ieee80211AkmSuite::Ft8021x;
    case Dot11AkmSuite::Dot11AkmSuiteFtPsk:
        return Ieee80211AkmSuite::FtPsk;
    case Dot11AkmSuite::Dot11AkmSuite8021xSha256:
        return Ieee80211AkmSuite::Ieee8021xSha256;
    case Dot11AkmSuite::Dot11AkmSuitePskSha256:
        return Ieee80211AkmSuite::PskSha256;
    case Dot11AkmSuite::Dot11AkmSuiteTdls:
        return Ieee80211AkmSuite::Tdls;
    case Dot11AkmSuite::Dot11AkmSuiteSae:
        return Ieee80211AkmSuite::Sae;
    case Dot11AkmSuite::Dot11AkmSuiteFtSae:
        return Ieee80211AkmSuite::FtSae;
    case Dot11AkmSuite::Dot11AkmSuiteApPeerKey:
        return Ieee80211AkmSuite::ApPeerKey;
    case Dot11AkmSuite::Dot11AkmSuite8021xSuiteB:
        return Ieee80211AkmSuite::Ieee8021xSuiteB;
    case Dot11AkmSuite::Dot11AkmSuite8021xSuiteB192:
        return Ieee80211AkmSuite::Ieee8021xSuiteB192;
    case Dot11AkmSuite::Dot11AkmSuiteFt8021xSha384:
        return Ieee80211AkmSuite::Ft8021xSha384;
    case Dot11AkmSuite::Dot11AkmSuiteFilsSha256:
        return Ieee80211AkmSuite::FilsSha256;
    case Dot11AkmSuite::Dot11AkmSuiteFilsSha384:
        return Ieee80211AkmSuite::FilsSha384;
    case Dot11AkmSuite::Dot11AkmSuiteFtFilsSha256:
        return Ieee80211AkmSuite::FtFilsSha256;
    case Dot11AkmSuite::Dot11AkmSuiteFtFilsSha384:
        return Ieee80211AkmSuite::FtFilsSha384;
    case Dot11AkmSuite::Dot11AkmSuiteOwe:
        return Ieee80211AkmSuite::Owe;
    case Dot11AkmSuite::Dot11AkmSuiteFtPskSha384:
        return Ieee80211AkmSuite::FtPskSha384;
    case Dot11AkmSuite::Dot11AkmSuitePskSha384:
        return Ieee80211AkmSuite::PskSha384;
    default:
        return Ieee80211AkmSuite::Reserved0;
    }
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

Ieee80211CipherSuite
NetRemoteCipherSuiteToIeeeCipherAlgorithm(Dot11CipherSuite cipherSuite)
{
    switch (cipherSuite) {
    case Dot11CipherSuite::Dot11CipherSuiteBipCmac128:
        return Ieee80211CipherSuite::BipCmac128;
    case Dot11CipherSuite::Dot11CipherSuiteBipCmac256:
        return Ieee80211CipherSuite::BipCmac256;
    case Dot11CipherSuite::Dot11CipherSuiteBipGmac128:
        return Ieee80211CipherSuite::BipGmac128;
    case Dot11CipherSuite::Dot11CipherSuiteBipGmac256:
        return Ieee80211CipherSuite::BipGmac256;
    case Dot11CipherSuite::Dot11CipherSuiteCcmp128:
        return Ieee80211CipherSuite::Ccmp128;
    case Dot11CipherSuite::Dot11CipherSuiteCcmp256:
        return Ieee80211CipherSuite::Ccmp256;
    case Dot11CipherSuite::Dot11CipherSuiteGcmp128:
        return Ieee80211CipherSuite::Gcmp128;
    case Dot11CipherSuite::Dot11CipherSuiteGcmp256:
        return Ieee80211CipherSuite::Gcmp256;
    case Dot11CipherSuite::Dot11CipherSuiteGroupAddressesTrafficNotAllowed:
        return Ieee80211CipherSuite::GroupAddressesTrafficNotAllowed;
    case Dot11CipherSuite::Dot11CipherSuiteTkip:
        return Ieee80211CipherSuite::Tkip;
    case Dot11CipherSuite::Dot11CipherSuiteUseGroup:
        return Ieee80211CipherSuite::UseGroup;
    case Dot11CipherSuite::Dot11CipherSuiteWep104:
        return Ieee80211CipherSuite::Wep104;
    case Dot11CipherSuite::Dot11CipherSuiteWep40:
        return Ieee80211CipherSuite::Wep40;
    default:
        return Ieee80211CipherSuite::Unknown;
    }
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
IAccessPointWeakToNetRemoteAccessPointResultItem(std::weak_ptr<Microsoft::Net::Wifi::IAccessPoint>& accessPointWeak)
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

std::unique_ptr<Microsoft::Net::Wifi::IAccessPointController>
IAccessPointWeakToAccessPointController(std::weak_ptr<Microsoft::Net::Wifi::IAccessPoint>& accessPointWeak)
{
    auto accessPoint = accessPointWeak.lock();
    if (accessPoint != nullptr) {
        return accessPoint->CreateController();
    } else {
        LOGE << "Failed to retrieve access point as it is no longer valid";
    }

    return nullptr;
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

using Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus;
using Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatusCode;

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
    auto accessPointWeak = m_accessPointManager->GetAccessPoint(request->accesspointid());
    auto accessPointController = detail::IAccessPointWeakToAccessPointController(accessPointWeak);
    if (!accessPointController) {
        return handleFailure(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeAccessPointInvalid, std::format("Failed to create controller for access point {}", request->accesspointid()));
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
        return handleFailure(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeOperationNotSupported, std::format("Failed to get capabilities for access point {}", request->accesspointid()));
    }

    // Set the Ieee80211 protocol.
    try {
        if (!accessPointController->SetProtocol(ieeeProtocol)) {
            return handleFailure(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeOperationNotSupported, std::format("Failed to set PHY type for access point {}", request->accesspointid()));
        }
    } catch (const AccessPointControllerException& apce) {
        LOGE << std::format("Failed to set Ieee80211 protocol for access point {} ({})", request->accesspointid(), apce.what());
        return handleFailure(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeOperationNotSupported, std::format("Failed to set PHY type for access point {}", request->accesspointid()));
    }

    status.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);
    response->set_accesspointid(request->accesspointid());
    *response->mutable_status() = std::move(status);

    return grpc::Status::OK;
}

using Microsoft::Net::Wifi::Dot11AccessPointAuthenticationConfiguration;
using Microsoft::Net::Wifi::Dot11AkmSuite;
using Microsoft::Net::Wifi::Dot11AkmSuiteConfiguration;
using Microsoft::Net::Wifi::Dot11AuthenticationAlgorithm;
using Microsoft::Net::Wifi::Dot11CipherSuite;

::grpc::Status
NetRemoteService::WifiAccessPointSetAuthenticationConfiguration([[maybe_unused]] ::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiAccessPointSetAuthenticationConfigurationRequest* request, ::Microsoft::Net::Remote::Wifi::WifiAccessPointSetAuthenticationConfigurationResult* response)
{
    LOGD << std::format("Received WifiAccessPointSetAuthenticationConfiguration request for access point id {}", request->accesspointid());

    WifiAccessPointOperationStatus status{};

    const auto& authenticationConfiguration = request->authenticationconfiguration();
    const auto& akmSuiteConfigurations = authenticationConfiguration.akmsuites();
    const auto& cipherSuites = authenticationConfiguration.ciphersuites();

    auto handleFailure = [&](WifiAccessPointOperationStatusCode statusCode, std::string statusMessage) {
        LOGE << statusMessage;

        status.set_code(statusCode);
        status.set_message(statusMessage);

        response->set_accesspointid(request->accesspointid());
        *response->mutable_status() = std::move(status);

        return grpc::Status::OK;
    };

    // Check for invalid provided AKM suites or cipher suites.
    if (std::empty(akmSuiteConfigurations) && std::empty(cipherSuites)) {
        return handleFailure(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter, "No AKM suite or cipher suite provided");
    } else if (!std::empty(akmSuiteConfigurations) && std::ranges::find(akmSuiteConfigurations, Dot11AkmSuite::Dot11AkmSuiteUnknown, &Dot11AkmSuiteConfiguration::akmsuite) != std::cend(akmSuiteConfigurations)) {
        return handleFailure(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter, "Invalid AKM suite provided");
    } else if (!std::empty(akmSuiteConfigurations) && std::ranges::find(akmSuiteConfigurations, Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmUnknown, &Dot11AkmSuiteConfiguration::authenticationalgorithm) != std::cend(akmSuiteConfigurations)) {
        return handleFailure(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter, "Invalid authentication algorithm provided");
    } else if (!std::empty(cipherSuites) && std::ranges::find(cipherSuites, Dot11CipherSuite::Dot11CipherSuiteUnknown) != std::cend(cipherSuites)) {
        return handleFailure(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter, "Invalid cipher suite provided");
    }

    // Create an AP controller for the requested AP.
    auto accessPointWeak = m_accessPointManager->GetAccessPoint(request->accesspointid());
    auto accessPointController = detail::IAccessPointWeakToAccessPointController(accessPointWeak);
    if (!accessPointController) {
        return handleFailure(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeAccessPointInvalid, std::format("Failed to create controller for access point {}", request->accesspointid()));
    }

    // Convert Dot11AkmSuiteConfigurations to Ieee equivalent values.
    std::vector<Microsoft::Net::Wifi::Ieee80211AkmSuite> ieeeAkmSuites;
    std::vector<Microsoft::Net::Wifi::Ieee80211AuthenticationAlgorithm> ieeeAuthenticationAlgorithms;
    std::ranges::transform(akmSuiteConfigurations, std::back_inserter(ieeeAkmSuites), [&](const Dot11AkmSuiteConfiguration& akmSuiteConfiguration) {
        return detail::NetRemoteAkmToIeee80211AkmSuite(akmSuiteConfiguration.akmsuite());
    });
    std::ranges::transform(akmSuiteConfigurations, std::back_inserter(ieeeAuthenticationAlgorithms), [&](const Dot11AkmSuiteConfiguration& akmSuiteConfiguration) {
        return detail::NetRemoteAuthenticationAlgorithmToIeeeAuthenticationAlgorithm(akmSuiteConfiguration.authenticationalgorithm());
    });
    // TODO: Store configuration values somehow.

    // TODO: Convert Dot11CipherSuites to Ieee80211CipherSuites.

    // Check if provided authentication configuration is supported by AP.
    try {
        auto accessPointCapabilities = accessPointController->GetCapabilities();
        const auto& supportedAkmSuites = accessPointCapabilities.AkmSuites;
        const auto& supportedCipherSuites = accessPointCapabilities.CipherSuites;

        if (!std::empty(akmSuiteConfigurations)) {
            // Remove unsupported AKM suites.
            for (const auto& ieeeAkmSuite : ieeeAkmSuites) {
                if (std::ranges::find(supportedAkmSuites, ieeeAkmSuite) == std::cend(supportedAkmSuites)) {
                    std::erase(ieeeAkmSuites, ieeeAkmSuite);
                }
            }
            // TODO: Do the same for other parts of Dot11AkmSuiteConfiguration.
        }

        if (!std::empty(cipherSuites)) {
            // TODO: Remove unsupported cipher suites.
        }
    } catch (const AccessPointControllerException& apce) {
        LOGE << std::format("Failed to get capabilities for access point {} ({})", request->accesspointid(), apce.what());
        return handleFailure(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeOperationNotSupported, std::format("Failed to get capabilities for access point {}", request->accesspointid()));
    }

    // Set the authentication configuration for the AP.
    try {
        if (!std::empty(akmSuiteConfigurations)) {
            if (!accessPointController->SetAkmSuites(ieeeAkmSuites)) {
                return handleFailure(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeOperationNotSupported, std::format("Failed to set AKM suites for access point {}", request->accesspointid()));
            }
        }

        if (!std::empty(cipherSuites)) {
            // TODO: Set cipher suites via accessPointController->SetCipherSuites.
        }
    } catch (const AccessPointControllerException& apce) {
        LOGE << std::format("Failed to set authentication configuration for access point {} ({})", request->accesspointid(), apce.what());
        return handleFailure(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeOperationNotSupported, std::format("Failed to set authentication configuration for access point {}", request->accesspointid()));
    }

    status.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);
    response->set_accesspointid(request->accesspointid());
    *response->mutable_status() = std::move(status);

    return grpc::Status::OK;
}

bool
NetRemoteService::ValidateWifiAccessPointEnableRequest(const ::Microsoft::Net::Remote::Wifi::WifiAccessPointEnableRequest* request, ::Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus& status)
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
