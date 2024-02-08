
#include <microsoft/net/wifi/Ieee80211Dot11Adapters.hxx>

namespace Microsoft::Net::Wifi
{
using Microsoft::Net::Wifi::Dot11PhyType;
using Microsoft::Net::Wifi::Ieee80211Protocol;

Dot11PhyType
ToDot11PhyType(const Ieee80211Protocol ieee80211Protocol) noexcept
{
    switch (ieee80211Protocol) {
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
FromDot11PhyType(const Dot11PhyType dot11PhyType) noexcept
{
    switch (dot11PhyType) {
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

using Microsoft::Net::Wifi::Dot11AuthenticationAlgorithm;
using Microsoft::Net::Wifi::Ieee80211AuthenticationAlgorithm;

Dot11AuthenticationAlgorithm
ToDot11AuthenticationAlgorithm(const Ieee80211AuthenticationAlgorithm ieeeAuthenticationAlgorithm) noexcept
{
    switch (ieeeAuthenticationAlgorithm) {
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
ToDot11AkmSuite(const Ieee80211AkmSuite ieeeAkmSuite) noexcept
{
    switch (ieeeAkmSuite) {
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

} // namespace Microsoft::Net::Wifi