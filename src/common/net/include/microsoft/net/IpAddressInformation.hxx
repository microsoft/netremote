
#ifndef IP_ADDRESS_INFORMATION_HXX
#define IP_ADDRESS_INFORMATION_HXX

#include <regex>
#include <string>
#include <string_view>

namespace Microsoft::Net
{
/**
 * @brief Determine if a string contains an IPv4 or IPv6 "any" address. The port is optional for both forms, and square
 * brackets are optional for the IPv6 form.
 *
 * @param ipAddressView The view of the string to check.
 * @return true If the string contains an IPv4 or IPv6 "any" address.
 * @return false If the string does not contain an IPv4 or IPv6 "any" address.
 */
inline bool
IsAnyAddress(std::string_view ipAddressView) noexcept
{
    const std::regex RegexIpv4AnyAddressWithPort{ "^0.0.0.0(:\\d+)?$" };
    const std::regex RegexIpv6AnyAddressWithPort{ "^\\[?::\\]?(:\\d+)?$" };
    const std::string ipAddress(ipAddressView);

    return std::regex_match(ipAddress, RegexIpv4AnyAddressWithPort) || std::regex_match(ipAddress, RegexIpv6AnyAddressWithPort);
}

/**
 * @brief The type of network interface.
 */
enum class NetworkInterfaceType {
    Unknown,
    Wifi,
    Bridge,
    Other,
};

/**
 * @brief The IP family of an IP address.
 */
enum class IpFamily {
    Unknown,
    Ipv4,
    Ipv6,
};

/**
 * @brief Information about an IP address needed by the server.
 */
struct IpAddressInformation
{
    IpFamily Family{ IpFamily::Unknown };
    NetworkInterfaceType InterfaceType{ NetworkInterfaceType::Unknown };

    auto
    operator<=>(const IpAddressInformation&) const = default;
};
} // namespace Microsoft::Net

namespace std
{
template <>
struct hash<Microsoft::Net::IpAddressInformation>
{
    std::size_t
    operator()(const Microsoft::Net::IpAddressInformation& ipInfo) const noexcept
    {
        return std::hash<int>{}(static_cast<int>(ipInfo.Family)) ^ std::hash<int>{}(static_cast<int>(ipInfo.InterfaceType));
    }
};
} // namespace std

#endif // IP_ADDRESS_INFORMATION_HXX
