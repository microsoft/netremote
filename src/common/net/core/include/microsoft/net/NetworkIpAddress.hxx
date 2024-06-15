
#ifndef NETWORK_IP_ADDRESS_HXX
#define NETWORK_IP_ADDRESS_HXX

#include <cstdint>
#include <functional>
#include <regex>
#include <string>

namespace Microsoft::Net
{
/**
 * @brief The network family on an IP address.
 */
enum class NetworkIpFamily {
    Unknown,
    Ipv4,
    Ipv6,
};

/**
 * @brief An network IP address with a prefix length.
 *
 * The format of the 'Address' value is dependent on the address family:
 *
 * - If 'Family' is 'Ipv4', then 'Address' is an IPv4 address in dotted decimal notation.
 * - If 'Family' is 'Ipv6', then 'Address' is an IPv6 address in colon-separated hexadecimal notation.
 */
struct NetworkIpAddress
{
    NetworkIpFamily Family{ NetworkIpFamily::Unknown };
    std::string Address;
    uint32_t PrefixLength;

    auto
    operator<=>(const NetworkIpAddress&) const = default;
};

/**
 * @brief Categories of IP addresses.
 */
enum class IpAddressCategory {
    Unknown,
    Ipv4,
    Ipv4Any,
    Ipv6Any,
};

/**
 * @brief Get the category of an IP address.
 * 
 * @param ipAddressView The view of the IP address to categorize.
 * @return IpAddressCategory The category of the IP address.
 */
inline IpAddressCategory
GetIpAddressCategory(std::string_view ipAddressView)
{
    const std::string ipAddress{ ipAddressView };

    const std::regex RegexIpv4AnyAddressWithOptionalPort{ "^0.0.0.0(:\\d+)?$" };
    if (std::regex_match(ipAddress, RegexIpv4AnyAddressWithOptionalPort))
    {
        return IpAddressCategory::Ipv4Any;
    }

    const std::regex RegexIpv6AnyAddressWithOptionalPort{ "^\\[?::\\]?(:\\d+)?$" };
    if (std::regex_match(ipAddress, RegexIpv6AnyAddressWithOptionalPort))
    {
        return IpAddressCategory::Ipv6Any;
    }

    const std::regex RegexIpv4AddressWithOptionalPort(R"(^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)(?::(\d|[1-9]\d{1,3}|[1-5]\d{4}|6[0-4]\d{3}|65[0-4]\d{2}|655[0-2]\d|6553[0-5]))?$)");
    if (std::regex_match(ipAddress, RegexIpv4AddressWithOptionalPort))
    {
        return IpAddressCategory::Ipv4;
    }

    return IpAddressCategory::Unknown;
}

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
    const auto ipAddressCategory = GetIpAddressCategory(ipAddressView);
    return ipAddressCategory == IpAddressCategory::Ipv4Any || ipAddressCategory == IpAddressCategory::Ipv6Any;
}
} // namespace Microsoft::Net

namespace std
{
template <>
struct hash<Microsoft::Net::NetworkIpAddress>
{
    std::size_t
    operator()(const Microsoft::Net::NetworkIpAddress& networkIpAddress) const noexcept
    {
        return std::hash<std::string>{}(networkIpAddress.Address) ^ std::hash<uint32_t>{}(networkIpAddress.PrefixLength) ^ std::hash<int>{}(static_cast<int>(networkIpAddress.Family));
    }
};
} // namespace std

#endif // NETWORK_IP_ADDRESS_HXX
