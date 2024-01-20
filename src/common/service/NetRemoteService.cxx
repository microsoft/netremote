
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
Microsoft::Net::Wifi::Dot11PhyType
IeeeProtocolToNetRemotePhyType(Microsoft::Net::Wifi::IeeeProtocol ieeeProtocol)
{
    using Microsoft::Net::Wifi::Dot11PhyType;
    using Microsoft::Net::Wifi::IeeeProtocol;

    Dot11PhyType phyType{ Dot11PhyType::Dot11PhyTypeUnknown };

    switch (ieeeProtocol) {
    case IeeeProtocol::B:
        phyType = Dot11PhyType::Dot11PhyTypeB;
        break;
    case IeeeProtocol::G:
        phyType = Dot11PhyType::Dot11PhyTypeG;
        break;
    case IeeeProtocol::N:
        phyType = Dot11PhyType::Dot11PhyTypeN;
        break;
    case IeeeProtocol::A:
        phyType = Dot11PhyType::Dot11PhyTypeA;
        break;
    case IeeeProtocol::AC:
        phyType = Dot11PhyType::Dot11PhyTypeAC;
        break;
    case IeeeProtocol::AD:
        phyType = Dot11PhyType::Dot11PhyTypeAD;
        break;
    case IeeeProtocol::AX:
        phyType = Dot11PhyType::Dot11PhyTypeAX;
        break;
    case IeeeProtocol::BE:
        phyType = Dot11PhyType::Dot11PhyTypeBE;
        break;
    default:
        break;
    }

    return phyType;
}

Microsoft::Net::Wifi::RadioBand
IeeeFrequencyBandToNetRemoteRadioBand(Microsoft::Net::Wifi::IeeeFrequencyBand ieeeFrequencyBand)
{
    using Microsoft::Net::Wifi::IeeeFrequencyBand;
    using Microsoft::Net::Wifi::RadioBand;

    RadioBand band{ RadioBand::RadioBandUnknown };

    switch (ieeeFrequencyBand) {
    case IeeeFrequencyBand::TwoPointFourGHz:
        band = RadioBand::RadioBandTwoPoint4GHz;
        break;
    case IeeeFrequencyBand::FiveGHz:
        band = RadioBand::RadioBandFiveGHz;
        break;
    case IeeeFrequencyBand::SixGHz:
        band = RadioBand::RadioBandSixGHz;
        break;
    default:
        break;
    }

    return band;
}

Microsoft::Net::Wifi::Dot11AuthenticationAlgorithm
IeeeAuthenticationAlgorithmToNetRemoteAuthenticationAlgorithm(Microsoft::Net::Wifi::IeeeAuthenticationAlgorithm ieeeAuthenticationAlgorithm)
{
    using Microsoft::Net::Wifi::Dot11AuthenticationAlgorithm;
    using Microsoft::Net::Wifi::IeeeAuthenticationAlgorithm;

    Dot11AuthenticationAlgorithm authenticationAlgorithm{ Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmUnknown };

    switch (ieeeAuthenticationAlgorithm) {
    case IeeeAuthenticationAlgorithm::OpenSystem:
        authenticationAlgorithm = Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmOpen;
        break;
    case IeeeAuthenticationAlgorithm::SharedKey:
        authenticationAlgorithm = Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmSharedKey;
        break;
    case IeeeAuthenticationAlgorithm::Sae:
        authenticationAlgorithm = Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmSae;
        break;
    // The following cases do not map to Dot11AuthenticationAlgorithm types. This appears to be because the Microsoft
    // authentication algorithm definitions do not map 1-1 with the 802.11 definitions. Instead, they more resemble an
    // 802.11 AKM. Fixing this requires a breaking API change, so will be deferred.
    //
    // case IeeeAuthenticationAlgorithm::FastBssTransition:
    // case IeeeAuthenticationAlgorithm::Fils:
    // case IeeeAuthenticationAlgorithm::FilsPfs:
    // case IeeeAuthenticationAlgorithm::FilsPublicKey:
    //
    default:
        break;
    }

    return authenticationAlgorithm;
}

Microsoft::Net::Wifi::Dot11CipherAlgorithm
IeeeCipherAlgorithmToNetRemoteCipherAlgorithm(Microsoft::Net::Wifi::IeeeCipherSuite ieeeCipherSuite)
{
    using Microsoft::Net::Wifi::Dot11CipherAlgorithm;
    using Microsoft::Net::Wifi::IeeeCipherSuite;

    Dot11CipherAlgorithm cipherAlgorithm{ Dot11CipherAlgorithm::Dot11CipherAlgorithmUnknown };

    switch (ieeeCipherSuite) {
    case IeeeCipherSuite::Unknown:
        cipherAlgorithm = Dot11CipherAlgorithm::Dot11CipherAlgorithmNone;
        break;
    case IeeeCipherSuite::Wep40:
        cipherAlgorithm = Dot11CipherAlgorithm::Dot11CipherAlgorithmWep40;
        break;
    case IeeeCipherSuite::Tkip:
        cipherAlgorithm = Dot11CipherAlgorithm::Dot11CipherAlgorithmTkip;
        break;
    // case IeeeCipherSuite::Ccmp128: // FIXME
    case IeeeCipherSuite::Wep104:
        cipherAlgorithm = Dot11CipherAlgorithm::Dot11CipherAlgorithmWep104;
        break;
    case IeeeCipherSuite::BipCmac128:
        cipherAlgorithm = Dot11CipherAlgorithm::Dot11CipherAlgorithmBipCmac128;
        break;
    case IeeeCipherSuite::Gcmp128:
        cipherAlgorithm = Dot11CipherAlgorithm::Dot11CipherAlgorithmGcmp128;
        break;
    case IeeeCipherSuite::Gcmp256:
        cipherAlgorithm = Dot11CipherAlgorithm::Dot11CipherAlgorithmGcmp256;
        break;
    case IeeeCipherSuite::Ccmp256:
        cipherAlgorithm = Dot11CipherAlgorithm::Dot11CipherAlgorithmCcmp256;
        break;
    case IeeeCipherSuite::BipGmac128:
        cipherAlgorithm = Dot11CipherAlgorithm::Dot11CipherAlgorithmBipGmac128;
        break;
    case IeeeCipherSuite::BipGmac256:
        cipherAlgorithm = Dot11CipherAlgorithm::Dot11CipherAlgorithmBipGmac256;
        break;
    case IeeeCipherSuite::BipCmac256:
        cipherAlgorithm = Dot11CipherAlgorithm::Dot11CipherAlgorithmBipCmac256;
        break;
    default:
        break;
    }

    return cipherAlgorithm;
}

Microsoft::Net::Wifi::AccessPointCapabilities
IeeeAccessPointCapabilitiesToNetRemoteAccessPointCapabilities(const Microsoft::Net::Wifi::Ieee80211AccessPointCapabilities& ieeeCapabilities)
{
    using Microsoft::Net::Wifi::AccessPointCapabilities;
    using Microsoft::Net::Wifi::Ieee80211AccessPointCapabilities;

    AccessPointCapabilities capabilities{};

    std::vector<Microsoft::Net::Wifi::Dot11PhyType> phyTypes(std::size(ieeeCapabilities.Protocols));
    std::ranges::transform(ieeeCapabilities.Protocols, std::begin(phyTypes), IeeeProtocolToNetRemotePhyType);

    *capabilities.mutable_phytypes() = {
        std::make_move_iterator(std::begin(phyTypes)),
        std::make_move_iterator(std::end(phyTypes))
    };

    std::vector<Microsoft::Net::Wifi::RadioBand> bands(std::size(ieeeCapabilities.FrequencyBands));
    std::ranges::transform(ieeeCapabilities.FrequencyBands, std::begin(bands), IeeeFrequencyBandToNetRemoteRadioBand);

    *capabilities.mutable_bands() = {
        std::make_move_iterator(std::begin(bands)),
        std::make_move_iterator(std::end(bands))
    };

    std::vector<Microsoft::Net::Wifi::Dot11AuthenticationAlgorithm> authenticationAlgorithms(std::size(ieeeCapabilities.AuthenticationAlgorithms));
    std::ranges::transform(ieeeCapabilities.AuthenticationAlgorithms, std::begin(authenticationAlgorithms), IeeeAuthenticationAlgorithmToNetRemoteAuthenticationAlgorithm);

    *capabilities.mutable_authenticationalgorithms() = {
        std::make_move_iterator(std::begin(authenticationAlgorithms)),
        std::make_move_iterator(std::end(authenticationAlgorithms))
    };

    std::vector<Microsoft::Net::Wifi::Dot11CipherAlgorithm> encryptionAlgorithms(std::size(ieeeCapabilities.EncryptionAlgorithms));
    std::ranges::transform(ieeeCapabilities.EncryptionAlgorithms, std::begin(encryptionAlgorithms), IeeeCipherAlgorithmToNetRemoteCipherAlgorithm);

    *capabilities.mutable_encryptionalgorithms() = {
        std::make_move_iterator(std::begin(encryptionAlgorithms)),
        std::make_move_iterator(std::end(encryptionAlgorithms))
    };

    return capabilities;
}

using Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsResultItem;
using Microsoft::Net::Wifi::AccessPointCapabilities;

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
    Microsoft::Net::Wifi::AccessPointCapabilities capabilities{};

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
using Microsoft::Net::Wifi::Dot11AuthenticationAlgorithm;
using Microsoft::Net::Wifi::Dot11CipherAlgorithm;
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
    if (configuration.encryptionalgorithm() == Dot11CipherAlgorithm::Dot11CipherAlgorithmUnknown) {
        status.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
        status.set_message("No encryption algorithm provided");
        return false;
    }
    if (std::empty(configuration.bands())) {
        status.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
        status.set_message("No radio bands provided");
        return false;
    }

    return true;
}
