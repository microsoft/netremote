
#include <algorithm>
#include <charconv>
#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>

#include <microsoft/net/wifi/Ieee80211.hxx>

namespace Microsoft::Net::Wifi
{
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

    if (std::size(macAddress) < std::tuple_size_v<Ieee80211MacAddress>) {
        return std::nullopt;
    }

    Ieee80211MacAddress result{};
    std::string_view macAddressView{ macAddress };
    for (std::size_t i = 0; i < std::size(macAddressView); i++) {
        const auto byteAsHex = macAddressView.substr(i * 2, 2); // 2 hex chars
        std::from_chars(std::data(byteAsHex), std::data(byteAsHex) + std::size(byteAsHex), result[i], 16);
    }

    return result;
}
} // namespace Microsoft::Net::Wifi
