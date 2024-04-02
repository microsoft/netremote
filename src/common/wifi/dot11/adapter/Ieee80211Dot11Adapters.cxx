
#include <algorithm>
#include <charconv>
#include <cstdlib>
#include <iterator>
#include <ranges>
#include <unordered_map>
#include <vector>

#include <microsoft/net/remote/protocol/NetRemoteWifi.pb.h>
#include <microsoft/net/remote/protocol/WifiCore.pb.h>
#include <microsoft/net/wifi/AccessPointOperationStatus.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <microsoft/net/wifi/Ieee80211AccessPointCapabilities.hxx>
#include <microsoft/net/wifi/Ieee80211Dot11Adapters.hxx>

namespace Microsoft::Net::Wifi
{
using Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatusCode;

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

std::vector<Dot11FrequencyBand>
ToDot11FrequencyBands(const std::vector<Ieee80211FrequencyBand>& ieee80211FrequencyBands) noexcept
{
    std::vector<Dot11FrequencyBand> dot11FrequencyBands(static_cast<std::size_t>(std::size(ieee80211FrequencyBands)));
    std::ranges::transform(ieee80211FrequencyBands, std::begin(dot11FrequencyBands), ToDot11FrequencyBand);

    return dot11FrequencyBands;
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

/**
 * @brief Convert an int-typed Dot11CipherSuite to its proper enum type.
 */
constexpr auto toDot11CipherSuite = [](const auto& cipherSuite) {
    return static_cast<Dot11CipherSuite>(cipherSuite);
};

/**
 * @brief Convert an int-typed Dot11AkmSuite to its proper enum type.
 */
constexpr auto toDot11AkmSuite = [](const auto& akmSuite) {
    return static_cast<Dot11AkmSuite>(akmSuite);
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

std::vector<Dot11AuthenticationAlgorithm>
ToDot11AuthenticationAlgorithms(const std::vector<Ieee80211AuthenticationAlgorithm>& ieee80211AuthenticationAlgorithms) noexcept
{
    std::vector<Dot11AuthenticationAlgorithm> dot11AuthenticationAlgorithms(static_cast<std::size_t>(std::size(ieee80211AuthenticationAlgorithms)));
    std::ranges::transform(ieee80211AuthenticationAlgorithms, std::begin(dot11AuthenticationAlgorithms), ToDot11AuthenticationAlgorithm);

    return dot11AuthenticationAlgorithms;
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
    case Ieee80211AkmSuite::Pasn:
        return Dot11AkmSuite::Dot11AkmSuitePasn;
    default:
        return Dot11AkmSuite::Dot11AkmSuiteUnknown;
    }
}

std::vector<Dot11AkmSuite>
ToDot11AkmSuites(const Dot11AccessPointConfiguration& dot11AccessPointConfiguration) noexcept
{
    std::vector<Dot11AkmSuite> dot11AkmSuites(static_cast<std::size_t>(std::size(dot11AccessPointConfiguration.akmsuites())));
    std::ranges::transform(dot11AccessPointConfiguration.akmsuites(), std::begin(dot11AkmSuites), detail::toDot11AkmSuite);

    return dot11AkmSuites;
}

std::vector<Dot11AkmSuite>
ToDot11AkmSuites(const std::vector<Ieee80211AkmSuite>& ieee80211AkmSuites) noexcept
{
    std::vector<Dot11AkmSuite> dot11AkmSuites(static_cast<std::size_t>(std::size(ieee80211AkmSuites)));
    std::ranges::transform(ieee80211AkmSuites, std::begin(dot11AkmSuites), ToDot11AkmSuite);

    return dot11AkmSuites;
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
    case Dot11AkmSuite::Dot11AkmSuitePasn:
        return Ieee80211AkmSuite::Pasn;
    default:
        return Ieee80211AkmSuite::Unknown;
    }
}

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

std::unordered_map<Dot11SecurityProtocol, std::vector<Dot11CipherSuite>>
ToDot11CipherSuiteConfigurations(const google::protobuf::RepeatedPtrField<Dot11CipherSuiteConfiguration>& dot11CipherSuiteConfigurations) noexcept
{
    std::unordered_map<Dot11SecurityProtocol, std::vector<Dot11CipherSuite>> dot11CipherSuiteConfigurationsStrong{};

    for (const auto& dot11CipherSuiteConfiguration : dot11CipherSuiteConfigurations) {
        std::vector<Dot11CipherSuite> dot11CipherSuites(static_cast<std::size_t>(dot11CipherSuiteConfiguration.ciphersuites_size()));
        std::ranges::transform(dot11CipherSuiteConfiguration.ciphersuites(), std::begin(dot11CipherSuites), detail::toDot11CipherSuite);
        dot11CipherSuiteConfigurationsStrong.emplace(dot11CipherSuiteConfiguration.securityprotocol(), std::move(dot11CipherSuites));
    }

    return dot11CipherSuiteConfigurationsStrong;
}

std::vector<Dot11CipherSuiteConfiguration>
ToDot11CipherSuiteConfigurations(const std::unordered_map<Ieee80211SecurityProtocol, std::vector<Ieee80211CipherSuite>>& ieee80211CipherSuiteConfigurations) noexcept
{
    std::vector<Dot11CipherSuiteConfiguration> dot11CipherSuiteConfigurations(std::size(ieee80211CipherSuiteConfigurations));

    std::ranges::transform(ieee80211CipherSuiteConfigurations, std::begin(dot11CipherSuiteConfigurations), [](const auto& ieee80211CipherSuiteConfiguration) {
        const auto& [ieee80211SecurityProtocol, ieee80211CipherSuites] = ieee80211CipherSuiteConfiguration;
        const auto dot11SecurityProtocol = ToDot11SecurityProtocol(ieee80211SecurityProtocol);
        std::vector<Dot11CipherSuite> dot11CipherSuites(std::size(ieee80211CipherSuites));
        std::ranges::transform(ieee80211CipherSuiteConfiguration.second, std::begin(dot11CipherSuites), ToDot11CipherSuite);

        Dot11CipherSuiteConfiguration dot11CipherSuiteConfiguration{};
        dot11CipherSuiteConfiguration.set_securityprotocol(dot11SecurityProtocol);
        *dot11CipherSuiteConfiguration.mutable_ciphersuites() = {
            std::make_move_iterator(std::begin(dot11CipherSuites)),
            std::make_move_iterator(std::end(dot11CipherSuites))
        };
        return dot11CipherSuiteConfiguration;
    });

    return dot11CipherSuiteConfigurations;
}

std::unordered_map<Ieee80211SecurityProtocol, std::vector<Ieee80211CipherSuite>>
FromDot11CipherSuiteConfigurations(const std::unordered_map<Dot11SecurityProtocol, std::vector<Dot11CipherSuite>>& dot11CipherSuiteConfigurations) noexcept
{
    std::unordered_map<Ieee80211SecurityProtocol, std::vector<Ieee80211CipherSuite>> ieee80211CipherSuiteConfigurations{};

    for (const auto& [dot11SecurityProtocol, dot11CipherSuites] : dot11CipherSuiteConfigurations) {
        std::vector<Ieee80211CipherSuite> ieee80211CipherSuites(std::size(dot11CipherSuites));
        std::ranges::transform(dot11CipherSuites, std::begin(ieee80211CipherSuites), FromDot11CipherSuite);
        ieee80211CipherSuiteConfigurations.emplace(FromDot11SecurityProtocol(dot11SecurityProtocol), std::move(ieee80211CipherSuites));
    }

    return ieee80211CipherSuiteConfigurations;
}

Dot11AccessPointCapabilities
ToDot11AccessPointCapabilities(const Ieee80211AccessPointCapabilities& ieee80211AccessPointCapabilities) noexcept
{
    Dot11AccessPointCapabilities dot11Capabilities{};

    std::vector<Dot11SecurityProtocol> securityProtocols(std::size(ieee80211AccessPointCapabilities.SecurityProtocols));
    std::ranges::transform(ieee80211AccessPointCapabilities.SecurityProtocols, std::begin(securityProtocols), ToDot11SecurityProtocol);

    *dot11Capabilities.mutable_securityprotocols() = {
        std::make_move_iterator(std::begin(securityProtocols)),
        std::make_move_iterator(std::end(securityProtocols))
    };

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

Ieee80211MacAddress
FromDot11MacAddress(const Dot11MacAddress& dot11MacAddress) noexcept
{
    Ieee80211MacAddress ieee80211MacAddress{};

    const auto& value = dot11MacAddress.value();
    if (std::size(value) >= std::size(ieee80211MacAddress)) {
        std::ranges::copy_n(std::cbegin(value), std::size(ieee80211MacAddress), std::begin(ieee80211MacAddress));
    }

    return ieee80211MacAddress;
}

Dot11MacAddress
ToDot11MacAddress(const Ieee80211MacAddress& ieee80211MacAddress) noexcept
{
    Dot11MacAddress dot11MacAddress{};

    dot11MacAddress.mutable_value()->assign(std::cbegin(ieee80211MacAddress), std::cend(ieee80211MacAddress));

    return dot11MacAddress;
}

Ieee80211RsnaPsk
FromDot11RsnaPsk(const Dot11RsnaPsk& dot11RsnaPsk) noexcept
{
    Ieee80211RsnaPsk ieee80211RsnaPsk{};

    if (dot11RsnaPsk.has_passphrase()) {
        const auto& pskPassphrase = dot11RsnaPsk.passphrase();
        ieee80211RsnaPsk = pskPassphrase;
    } else if (dot11RsnaPsk.has_value()) {
        const auto& pskValue = dot11RsnaPsk.value();

        // Ensure the variant holds the correct type by calling emplace with the corresponding index. Unfortunately
        // since the underlying type is a std::array which doesn't have a constructor, it can't be initialized with
        // a container via emplace args or variant constructor, so the data is explicitly copied with copy_n below,
        // which is what would happen for "array construction" anyway.
        auto& pskValueRaw = ieee80211RsnaPsk.emplace<1>();

        if (pskValue.has_hex()) {
            const auto& pskHex = pskValue.hex();
            // Ensure the hex string is at least twice the length of the value array (2 hex characters per byte).
            if (std::size(pskHex) >= std::size(ieee80211RsnaPsk.Value()) * 2) {
                std::string_view pskHexView{ pskHex };
                std::vector<uint8_t> pskValueRawV(std::size(pskValueRaw));
                for (std::size_t i = 0; i < std::size(pskValueRaw); i++) {
                    const auto byteAsHex = pskHexView.substr(i * 2, 2); // 2 hex chars
                    std::from_chars(std::data(byteAsHex), std::data(byteAsHex) + std::size(byteAsHex), pskValueRaw[i], 16);
                }
            }
        } else if (pskValue.has_raw()) {
            const auto& pskRaw = pskValue.raw();
            if (std::size(pskRaw) >= std::size(pskValueRaw)) {
                std::ranges::copy_n(std::cbegin(pskRaw), static_cast<long>(std::size(pskValueRaw)), std::begin(pskValueRaw));
            }
        } else {
            ieee80211RsnaPsk = {};
        }
    }

    return ieee80211RsnaPsk;
}

Dot11RsnaPsk
ToDot11RsnaPsk(const Ieee80211RsnaPsk& ieee80211RnsaPsk) noexcept
{
    Dot11RsnaPsk dot11RsnaPsk{};

    switch (ieee80211RnsaPsk.Encoding()) {
    case Ieee80211RsnaPskEncoding::Passphrase: {
        const auto& pskPassphrase = ieee80211RnsaPsk.Passphrase();
        *dot11RsnaPsk.mutable_passphrase() = pskPassphrase;
        break;
    }
    case Ieee80211RsnaPskEncoding::Value: {
        const auto& pskValue = ieee80211RnsaPsk.Value();
        dot11RsnaPsk.mutable_value()->mutable_raw()->assign(std::cbegin(pskValue), std::cend(pskValue));
        break;
    }
    default: {
        break;
    }
    }

    return dot11RsnaPsk;
}

Ieee80211RsnaPassword
FromDot11RsnaPassword(const Dot11RsnaPassword& dot11RsnaPassword) noexcept
{
    Ieee80211RsnaPassword ieee80211RsnaPassword{};

    ieee80211RsnaPassword.Credential = dot11RsnaPassword.credential();

    if (dot11RsnaPassword.has_peermacaddress()) {
        ieee80211RsnaPassword.PeerMacAddress = FromDot11MacAddress(dot11RsnaPassword.peermacaddress());
    }
    if (!std::empty(dot11RsnaPassword.passwordid())) {
        ieee80211RsnaPassword.PasswordId = dot11RsnaPassword.passwordid();
    }

    return ieee80211RsnaPassword;
}

Dot11RsnaPassword
ToDot11RsnaPassword(const Ieee80211RsnaPassword& ieee80211RsnaPassword) noexcept
{
    Dot11RsnaPassword dot11RsnaPassword{};

    *dot11RsnaPassword.mutable_credential() = ieee80211RsnaPassword.Credential;

    if (ieee80211RsnaPassword.PeerMacAddress.has_value()) {
        *dot11RsnaPassword.mutable_peermacaddress() = ToDot11MacAddress(ieee80211RsnaPassword.PeerMacAddress.value());
    }
    if (ieee80211RsnaPassword.PasswordId.has_value()) {
        dot11RsnaPassword.set_passwordid(ieee80211RsnaPassword.PasswordId.value());
    }

    return dot11RsnaPassword;
}

Ieee80211AuthenticationDataPsk
FromDot11AuthenticationDataPsk(const Dot11AuthenticationDataPsk& dot11AuthenticationDataPsk) noexcept
{
    Ieee80211AuthenticationDataPsk ieee80211AuthenticationDataPsk{};

    if (dot11AuthenticationDataPsk.has_psk()) {
        ieee80211AuthenticationDataPsk.Psk = FromDot11RsnaPsk(dot11AuthenticationDataPsk.psk());
    }

    return ieee80211AuthenticationDataPsk;
}

Dot11AuthenticationDataPsk
ToDot11AuthenticationDataPsk(const Ieee80211AuthenticationDataPsk& ieee80211AuthenticationDataPsk) noexcept
{
    Dot11AuthenticationDataPsk dot11AuthenticationDataPsk{};

    *dot11AuthenticationDataPsk.mutable_psk() = ToDot11RsnaPsk(ieee80211AuthenticationDataPsk.Psk);

    return dot11AuthenticationDataPsk;
}

Ieee80211AuthenticationDataSae
FromDot11AuthenticationDataSae(const Dot11AuthenticationDataSae& dot11AuthenticationDataSae) noexcept
{
    Ieee80211AuthenticationDataSae ieee80211AuthenticationDataSae{};

    std::vector<Ieee80211RsnaPassword> ieee80211RsnaPasswords(static_cast<std::size_t>(dot11AuthenticationDataSae.passwords_size()));
    std::ranges::transform(dot11AuthenticationDataSae.passwords(), std::begin(ieee80211RsnaPasswords), FromDot11RsnaPassword);
    ieee80211AuthenticationDataSae.Passwords = std::move(ieee80211RsnaPasswords);

    return ieee80211AuthenticationDataSae;
}

Dot11AuthenticationDataSae
ToDot11AuthenticationDataSae(const Ieee80211AuthenticationDataSae& ieee80211AuthenticationDataSae) noexcept
{
    Dot11AuthenticationDataSae dot11AuthenticationDataSae{};

    std::vector<Dot11RsnaPassword> dot11RsnaPasswords(std::size(ieee80211AuthenticationDataSae.Passwords));
    std::ranges::transform(ieee80211AuthenticationDataSae.Passwords, std::begin(dot11RsnaPasswords), ToDot11RsnaPassword);
    *dot11AuthenticationDataSae.mutable_passwords() = {
        std::make_move_iterator(std::begin(dot11RsnaPasswords)),
        std::make_move_iterator(std::end(dot11RsnaPasswords))
    };

    return dot11AuthenticationDataSae;
}

Ieee80211AuthenticationData
FromDot11AuthenticationData(const Dot11AuthenticationData& dot11AuthenticationData) noexcept
{
    Ieee80211AuthenticationData ieee80211AuthenticationData{};

    if (dot11AuthenticationData.has_psk()) {
        ieee80211AuthenticationData.Psk = FromDot11AuthenticationDataPsk(dot11AuthenticationData.psk());
    }
    if (dot11AuthenticationData.has_sae()) {
        ieee80211AuthenticationData.Sae = FromDot11AuthenticationDataSae(dot11AuthenticationData.sae());
    }

    return ieee80211AuthenticationData;
}

Dot11AuthenticationData
ToDot11AuthenticationData(const Ieee80211AuthenticationData& ieee80211AuthenticationData) noexcept
{
    Dot11AuthenticationData dot11AuthenticationData{};

    if (ieee80211AuthenticationData.Psk.has_value()) {
        *dot11AuthenticationData.mutable_psk() = ToDot11AuthenticationDataPsk(ieee80211AuthenticationData.Psk.value());
    }
    if (ieee80211AuthenticationData.Sae.has_value()) {
        *dot11AuthenticationData.mutable_sae() = ToDot11AuthenticationDataSae(ieee80211AuthenticationData.Sae.value());
    }

    return dot11AuthenticationData;
}

} // namespace Microsoft::Net::Wifi
