
#ifndef IP_ADDRESS_INFORMATION_HXX
#define IP_ADDRESS_INFORMATION_HXX

#include <string_view>

namespace Microsoft::Net
{
constexpr auto Ipv4AnyAddress{ "0.0.0.0" };
constexpr auto Ipv6AnyAddress{ "::" };

constexpr bool
IsAnyAddress(std::string_view ipAddress) noexcept
{
    // TODO: validate this logic.
    return ipAddress.contains(Ipv4AnyAddress) || ipAddress.starts_with(Ipv6AnyAddress);
}

/**
 * @brief The type of network interface.
 */
enum class NetworkInterfaceType {
    Unknown,
    Wifi,
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
