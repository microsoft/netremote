
#include <algorithm>
#include <charconv>
#include <format>
#include <optional>
#include <string>
#include <string_view>

#include <microsoft/net/wifi/Ieee80211.hxx>
#include <strings/StringParsing.hxx>

namespace Microsoft::Net::Wifi
{
std::string
Ieee80211SecurityProtocolToString(const Ieee80211SecurityProtocol& securityProtocol)
{
    switch (securityProtocol) {
    case Ieee80211SecurityProtocol::Wpa:
        return "Wpa";
    case Ieee80211SecurityProtocol::Wpa2:
        return "Wpa2";
    case Ieee80211SecurityProtocol::Wpa3:
        return "Wpa3";
    default:
        return "Unknown";
    }
}

std::string
Ieee80211FrequencyBandToString(const Ieee80211FrequencyBand& frequencyBand)
{
    switch (frequencyBand) {
    case Ieee80211FrequencyBand::TwoPointFourGHz:
        return "TwoPointFourGHz";
    case Ieee80211FrequencyBand::FiveGHz:
        return "FiveGHz";
    case Ieee80211FrequencyBand::SixGHz:
        return "SixGHz";
    default:
        return "Unknown"; 
    }
}

std::string
Ieee80211AuthenticationAlgorithmToString(const Ieee80211AuthenticationAlgorithm& authenticationAlgorithm)
{
    switch (authenticationAlgorithm) {
    case Ieee80211AuthenticationAlgorithm::OpenSystem:
        return "OpenSystem";
    case Ieee80211AuthenticationAlgorithm::SharedKey:
        return "SharedKey";
    case Ieee80211AuthenticationAlgorithm::FastBssTransition:
        return "FastBssTransition";
    case Ieee80211AuthenticationAlgorithm::Sae:
        return "Sae";
    case Ieee80211AuthenticationAlgorithm::Fils:
        return "Fils";
    case Ieee80211AuthenticationAlgorithm::FilsPfs:
        return "FilsPfs";
    case Ieee80211AuthenticationAlgorithm::FilsPublicKey:
        return "FilsPublicKey";
    case Ieee80211AuthenticationAlgorithm::VendorSpecific:
        return "VendorSpecific";
    default:
        return "Unknown";
    }
}

std::string
Ieee80211AkmSuiteToString(const Ieee80211AkmSuite& akmSuite)
{
    switch (akmSuite) {
    case Ieee80211AkmSuite::Reserved0:
        return "Reserved0";
    case Ieee80211AkmSuite::Ieee8021x:
        return "Ieee8021x";
    case Ieee80211AkmSuite::Psk:
        return "Psk";
    case Ieee80211AkmSuite::Ft8021x:
        return "Ft8021x";
    case Ieee80211AkmSuite::FtPsk:
        return "FtPsk";
    case Ieee80211AkmSuite::Ieee8021xSha256:
        return "Ieee8021xSha256";
    case Ieee80211AkmSuite::PskSha256:
        return "PskSha256";
    case Ieee80211AkmSuite::Tdls:
        return "Tdls";
    case Ieee80211AkmSuite::Sae:
        return "Sae";
    case Ieee80211AkmSuite::FtSae:
        return "FtSae";
    case Ieee80211AkmSuite::ApPeerKey:
        return "ApPeerKey";
    case Ieee80211AkmSuite::Ieee8021xSuiteB:
        return "Ieee8021xSuiteB";
    case Ieee80211AkmSuite::Ieee8011xSuiteB192:
        return "Ieee8021xSuiteB192";
    case Ieee80211AkmSuite::Ft8021xSha384:
        return "Ft8021xSha384";
    case Ieee80211AkmSuite::FilsSha256:
        return "FilsSha256";
    case Ieee80211AkmSuite::FilsSha384:
        return "FilsSha384";
    case Ieee80211AkmSuite::FtFilsSha256:
        return "FtFilsSha256";
    case Ieee80211AkmSuite::FtFilsSha384:
        return "FtFilsSha384";
    case Ieee80211AkmSuite::Owe:
        return "Owe";
    case Ieee80211AkmSuite::FtPskSha384:
        return "FtPskSha384";
    case Ieee80211AkmSuite::PskSha384:
        return "PskSha384";
    case Ieee80211AkmSuite::Pasn:
        return "Pasn";
    default:
        return "Unknown";
    }
}

std::string
Ieee80211CipherSuiteToString(const Ieee80211CipherSuite& cipherSuite)
{
    switch (cipherSuite) {
    case Ieee80211CipherSuite::BipCmac128:
        return "BipCmac128";
    case Ieee80211CipherSuite::BipCmac256:
        return "BipCmac256";
    case Ieee80211CipherSuite::BipGmac128:
        return "BipGmac128";
    case Ieee80211CipherSuite::BipGmac256:
        return "BipGmac256";
    case Ieee80211CipherSuite::Ccmp128:
        return "Ccmp128";
    case Ieee80211CipherSuite::Ccmp256:
        return "Ccmp256";
    case Ieee80211CipherSuite::Gcmp128:
        return "Gcmp128";
    case Ieee80211CipherSuite::Gcmp256:
        return "Gcmp256";
    case Ieee80211CipherSuite::GroupAddressesTrafficNotAllowed:
        return "GroupAddressesTrafficNotAllowed";
    case Ieee80211CipherSuite::Tkip:
        return "Tkip";
    case Ieee80211CipherSuite::UseGroup:
        return "UseGroup";
    case Ieee80211CipherSuite::Wep104:
        return "Wep104";
    case Ieee80211CipherSuite::Wep40:
        return "Wep40";
    default:
        return "Unknown";
    }
}

std::string
Ieee80211MacAddressToString(const Ieee80211MacAddress& macAddress)
{
    return std::format("{:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}", macAddress[0], macAddress[1], macAddress[2], macAddress[3], macAddress[4], macAddress[5]);
}

std::optional<Ieee80211MacAddress>
Ieee80211MacAddressFromString(std::string macAddress)
{
    constexpr auto isDelimeter = [](const char c) {
        return c == ':' || c == '-';
    };

    const auto eraseRange = std::ranges::remove_if(macAddress, isDelimeter);
    if (!std::empty(eraseRange)) {
        macAddress.erase(std::begin(eraseRange), std::end(eraseRange));
    }

    Ieee80211MacAddress result{};
    if (!Strings::ParseHexString(macAddress, result)) {
        return std::nullopt;
    }

    return result;
}
} // namespace Microsoft::Net::Wifi
