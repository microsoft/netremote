
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <ranges>
#include <vector>

#include <microsoft/net/remote/protocol/NetRemoteWifi.pb.h>
#include <microsoft/net/remote/protocol/WifiCore.pb.h>
#include <microsoft/net/wifi/AccessPointOperationStatus.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <microsoft/net/wifi/Ieee80211AccessPointCapabilities.hxx>
#include <microsoft/net/wifi/Ieee80211Dot11Adapters.hxx>
#include <notstd/Exceptions.hxx>

namespace Microsoft::Net::Wifi
{
using Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatusCode;
using Microsoft::Net::Wifi::AccessPointOperationStatusCode;

WifiAccessPointOperationStatusCode
ToDot11AccessPointOperationStatusCode(AccessPointOperationStatusCode& accessPointOperationStatusCode) noexcept
{
    switch (accessPointOperationStatusCode) {
    case AccessPointOperationStatusCode::Succeeded:
        return WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded;
    case AccessPointOperationStatusCode::InvalidParameter:
        return WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter;
    case AccessPointOperationStatusCode::AccessPointInvalid:
        return WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeAccessPointInvalid;
    case AccessPointOperationStatusCode::AccessPointNotEnabled:
        return WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeAccessPointNotEnabled;
    case AccessPointOperationStatusCode::OperationNotSupported:
        return WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeOperationNotSupported;
    case AccessPointOperationStatusCode::InternalError:
        return WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInternalError;
    case AccessPointOperationStatusCode::Unknown:
    default:
        return WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeUnknown;
    }
}

AccessPointOperationStatusCode
FromDot11AccessPointOperationStatusCode(WifiAccessPointOperationStatusCode wifiAccessPointOperationStatusCode) noexcept
{
    switch (wifiAccessPointOperationStatusCode) {
    case WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded:
        return AccessPointOperationStatusCode::Succeeded;
    case WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter:
        return AccessPointOperationStatusCode::InvalidParameter;
    case WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeAccessPointInvalid:
        return AccessPointOperationStatusCode::AccessPointInvalid;
    case WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeAccessPointNotEnabled:
        return AccessPointOperationStatusCode::AccessPointNotEnabled;
    case WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeOperationNotSupported:
        return AccessPointOperationStatusCode::OperationNotSupported;
    case WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInternalError:
        return AccessPointOperationStatusCode::InternalError;
    case WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeUnknown:
    default:
        return AccessPointOperationStatusCode::Unknown;
    }
}

using Microsoft::Net::Wifi::Dot11SecurityProtocol;
using Microsoft::Net::Wifi::Ieee80211SecurityProtocol;

Dot11SecurityProtocol
ToDot11SecurityProtocol(Ieee80211SecurityProtocol ieee80211SecurityProtocol) noexcept
{
    switch (ieee80211SecurityProtocol) {
    case Ieee80211SecurityProtocol::Wpa:
        return Dot11SecurityProtocol::Dot11SecurityProtocolWpa1;
    case Ieee80211SecurityProtocol::Wpa2:
        return Dot11SecurityProtocol::Dot11SecurityProtocolWpa2;
    case Ieee80211SecurityProtocol::Wpa3:
        return Dot11SecurityProtocol::Dot11SecurityProtocolWpa3;
    case Ieee80211SecurityProtocol::Unknown:
        [[fallthrough]];
    default:
        return Dot11SecurityProtocol::Dot11SecurityProtocolUnknown;
    }
}

Ieee80211SecurityProtocol
FromDot11SecurityProtocol(Dot11SecurityProtocol dot11SecurityProtocol) noexcept
{
    switch (dot11SecurityProtocol) {
    case Dot11SecurityProtocol::Dot11SecurityProtocolWpa1:
        return Ieee80211SecurityProtocol::Wpa;
    case Dot11SecurityProtocol::Dot11SecurityProtocolWpa2:
        return Ieee80211SecurityProtocol::Wpa2;
    case Dot11SecurityProtocol::Dot11SecurityProtocolWpa3:
        return Ieee80211SecurityProtocol::Wpa3;
    case Dot11SecurityProtocol::Dot11SecurityProtocolUnknown:
        [[fallthrough]];
    default:
        return Ieee80211SecurityProtocol::Unknown;
    }
}

using Microsoft::Net::Wifi::Dot11PhyType;
using Microsoft::Net::Wifi::Ieee80211PhyType;

Dot11PhyType
ToDot11PhyType(const Ieee80211PhyType ieee80211PhyType) noexcept
{
    switch (ieee80211PhyType) {
    case Ieee80211PhyType::Unknown:
        return Dot11PhyType::Dot11PhyTypeUnknown;
    case Ieee80211PhyType::B:
        return Dot11PhyType::Dot11PhyTypeB;
    case Ieee80211PhyType::G:
        return Dot11PhyType::Dot11PhyTypeG;
    case Ieee80211PhyType::N:
        return Dot11PhyType::Dot11PhyTypeN;
    case Ieee80211PhyType::A:
        return Dot11PhyType::Dot11PhyTypeA;
    case Ieee80211PhyType::AC:
        return Dot11PhyType::Dot11PhyTypeAC;
    case Ieee80211PhyType::AD:
        return Dot11PhyType::Dot11PhyTypeAD;
    case Ieee80211PhyType::AX:
        return Dot11PhyType::Dot11PhyTypeAX;
    case Ieee80211PhyType::BE:
        return Dot11PhyType::Dot11PhyTypeBE;
    }

    return Dot11PhyType::Dot11PhyTypeUnknown;
}

Ieee80211PhyType
FromDot11PhyType(const Dot11PhyType dot11PhyType) noexcept
{
    switch (dot11PhyType) {
    case Dot11PhyType::Dot11PhyTypeB:
        return Ieee80211PhyType::B;
    case Dot11PhyType::Dot11PhyTypeG:
        return Ieee80211PhyType::G;
    case Dot11PhyType::Dot11PhyTypeN:
        return Ieee80211PhyType::N;
    case Dot11PhyType::Dot11PhyTypeA:
        return Ieee80211PhyType::A;
    case Dot11PhyType::Dot11PhyTypeAC:
        return Ieee80211PhyType::AC;
    case Dot11PhyType::Dot11PhyTypeAD:
        return Ieee80211PhyType::AD;
    case Dot11PhyType::Dot11PhyTypeAX:
        return Ieee80211PhyType::AX;
    case Dot11PhyType::Dot11PhyTypeBE:
        return Ieee80211PhyType::BE;
    default:
        return Ieee80211PhyType::Unknown;
    }
}

using Microsoft::Net::Wifi::Dot11FrequencyBand;
using Microsoft::Net::Wifi::Ieee80211FrequencyBand;

Dot11FrequencyBand
ToDot11FrequencyBand(const Ieee80211FrequencyBand ieee80211FrequencyBand) noexcept
{
    switch (ieee80211FrequencyBand) {
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
FromDot11FrequencyBand(const Dot11FrequencyBand dot11FrequencyBand) noexcept
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

using Microsoft::Net::Remote::Wifi::WifiAccessPointSetFrequencyBandsRequest;

namespace detail
{
/**
 * protobuf encodes enums in repeated fields as 'int' instead of the enum type itself. So, the below are simple
 * functions to convert a repeated field of int type to the associated enum type.
 */

/**
 * @brief Convert an int-typed Dot11AuthenticationAlgorithm to its proper enum type.
 */
constexpr auto toDot11AuthenticationAlgorithm = [](const auto& authenticationAlgorithm) {
    return static_cast<Dot11AuthenticationAlgorithm>(authenticationAlgorithm);
};

/**
 * @brief Convert an int-typed Dot11FrequencyBand to its proper enum type.
 */
constexpr auto toDot11FrequencyBand = [](const auto& frequencyBand) {
    return static_cast<Dot11FrequencyBand>(frequencyBand);
};
} // namespace detail

std::vector<Dot11AuthenticationAlgorithm>
ToDot11AuthenticationAlgorithms(const Dot11AccessPointConfiguration& dot11AccessPointConfiguration) noexcept
{
    std::vector<Dot11AuthenticationAlgorithm> dot11AuthenticationAlgorithms(static_cast<std::size_t>(std::size(dot11AccessPointConfiguration.authenticationalgorithms())));
    std::ranges::transform(dot11AccessPointConfiguration.authenticationalgorithms(), std::begin(dot11AuthenticationAlgorithms), detail::toDot11AuthenticationAlgorithm);

    return dot11AuthenticationAlgorithms;
}

std::vector<Dot11FrequencyBand>
ToDot11FrequencyBands(const WifiAccessPointSetFrequencyBandsRequest& request) noexcept
{
    std::vector<Dot11FrequencyBand> dot11FrequencyBands(static_cast<std::size_t>(std::size(request.frequencybands())));
    std::ranges::transform(request.frequencybands(), std::begin(dot11FrequencyBands), detail::toDot11FrequencyBand);

    return dot11FrequencyBands;
    // TODO: for some reason, std::ranges::to is not being found for clang. Once resolved, update to the following:
    // return request.frequencybands() | std::views::transform(toDot11FrequencyBand) | std::ranges::to<std::vector>();
}

std::vector<Dot11FrequencyBand>
ToDot11FrequencyBands(const Dot11AccessPointConfiguration& dot11AccessPointConfiguration) noexcept
{
    std::vector<Dot11FrequencyBand> dot11FrequencyBands(static_cast<std::size_t>(std::size(dot11AccessPointConfiguration.frequencybands())));
    std::ranges::transform(dot11AccessPointConfiguration.frequencybands(), std::begin(dot11FrequencyBands), detail::toDot11FrequencyBand);

    return dot11FrequencyBands;
}

std::vector<Ieee80211FrequencyBand>
FromDot11SetFrequencyBandsRequest(const WifiAccessPointSetFrequencyBandsRequest& request)
{
    std::vector<Ieee80211FrequencyBand> ieee80211FrequencyBands(static_cast<std::size_t>(std::size(request.frequencybands())));
    std::ranges::transform(request.frequencybands() | std::views::transform(detail::toDot11FrequencyBand), std::begin(ieee80211FrequencyBands), FromDot11FrequencyBand);

    return ieee80211FrequencyBands;
}

using Microsoft::Net::Wifi::Dot11AuthenticationAlgorithm;
using Microsoft::Net::Wifi::Ieee80211AuthenticationAlgorithm;

Dot11AuthenticationAlgorithm
ToDot11AuthenticationAlgorithm(const Ieee80211AuthenticationAlgorithm ieee80211AuthenticationAlgorithm) noexcept
{
    switch (ieee80211AuthenticationAlgorithm) {
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
    case Ieee80211AuthenticationAlgorithm::Unknown:
    default:
        return Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmUnknown;
    }
}

Ieee80211AuthenticationAlgorithm
FromDot11AuthenticationAlgorithm(const Dot11AuthenticationAlgorithm dot11AuthenticationAlgorithm) noexcept
{
    switch (dot11AuthenticationAlgorithm) {
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
    case Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmUnknown:
    default:
        return Ieee80211AuthenticationAlgorithm::Unknown;
    }
}

using Microsoft::Net::Wifi::Dot11AkmSuite;
using Microsoft::Net::Wifi::Ieee80211AkmSuite;

Dot11AkmSuite
ToDot11AkmSuite(const Ieee80211AkmSuite ieee80211AkmSuite) noexcept
{
    switch (ieee80211AkmSuite) {
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
    default:
        return Dot11AkmSuite::Dot11AkmSuiteUnknown;
    }
}

Ieee80211AkmSuite
FromDot11AkmSuite(const Dot11AkmSuite dot11AkmSuite) noexcept
{
    switch (dot11AkmSuite) {
    case Dot11AkmSuite::Dot11AkmSuiteReserved0:
        return Ieee80211AkmSuite::Reserved0;
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
        return Ieee80211AkmSuite::Ieee8011xSuiteB192;
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
        return Ieee80211AkmSuite::Reserved0; // FIXME: this needs to be an invalid value instead
    }
}

using Microsoft::Net::Wifi::Dot11CipherSuite;
using Microsoft::Net::Wifi::Ieee80211CipherSuite;

Dot11CipherSuite
ToDot11CipherSuite(const Ieee80211CipherSuite ieee80211CipherSuite) noexcept
{
    switch (ieee80211CipherSuite) {
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
    case Ieee80211CipherSuite::Unknown:
    default:
        return Dot11CipherSuite::Dot11CipherSuiteUnknown;
    }
}

Ieee80211CipherSuite
FromDot11CipherSuite(const Dot11CipherSuite dot11CipherSuite) noexcept
{
    switch (dot11CipherSuite) {
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

using Microsoft::Net::Wifi::Dot11AccessPointCapabilities;
using Microsoft::Net::Wifi::Ieee80211AccessPointCapabilities;

Dot11AccessPointCapabilities
ToDot11AccessPointCapabilities(const Ieee80211AccessPointCapabilities& ieee80211AccessPointCapabilities) noexcept
{
    Dot11AccessPointCapabilities dot11Capabilities{};

    std::vector<Dot11SecurityProtocol> securityProtocols(std::size(ieee80211AccessPointCapabilities.SecurityProtocols));

    std::vector<Dot11PhyType> phyTypes(std::size(ieee80211AccessPointCapabilities.PhyTypes));
    std::ranges::transform(ieee80211AccessPointCapabilities.PhyTypes, std::begin(phyTypes), ToDot11PhyType);

    *dot11Capabilities.mutable_phytypes() = {
        std::make_move_iterator(std::begin(phyTypes)),
        std::make_move_iterator(std::end(phyTypes))
    };

    std::vector<Dot11FrequencyBand> bands(std::size(ieee80211AccessPointCapabilities.FrequencyBands));
    std::ranges::transform(ieee80211AccessPointCapabilities.FrequencyBands, std::begin(bands), ToDot11FrequencyBand);

    *dot11Capabilities.mutable_frequencybands() = {
        std::make_move_iterator(std::begin(bands)),
        std::make_move_iterator(std::end(bands))
    };

    std::vector<Dot11AkmSuite> akmSuites(std::size(ieee80211AccessPointCapabilities.AkmSuites));
    std::ranges::transform(ieee80211AccessPointCapabilities.AkmSuites, std::begin(akmSuites), ToDot11AkmSuite);

    *dot11Capabilities.mutable_akmsuites() = {
        std::make_move_iterator(std::begin(akmSuites)),
        std::make_move_iterator(std::end(akmSuites))
    };

    std::vector<Dot11CipherSuite> cipherSuites(std::size(ieee80211AccessPointCapabilities.CipherSuites));
    std::ranges::transform(ieee80211AccessPointCapabilities.CipherSuites, std::begin(cipherSuites), ToDot11CipherSuite);

    *dot11Capabilities.mutable_ciphersuites() = {
        std::make_move_iterator(std::begin(cipherSuites)),
        std::make_move_iterator(std::end(cipherSuites))
    };

    return dot11Capabilities;
}

Ieee80211AccessPointCapabilities
FromDot11AccessPointCapabilities([[maybe_unused]] const Dot11AccessPointCapabilities& dot11AccessPointCapabilities) /* noexcept */
{
    throw notstd::NotImplementedException();
}

} // namespace Microsoft::Net::Wifi
