
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
std::string_view
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
