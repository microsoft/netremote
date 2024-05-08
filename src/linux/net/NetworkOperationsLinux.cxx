
#include <algorithm>
#include <ranges>
#include <string>
#include <string_view>
#include <sys/socket.h>
#include <unordered_map>
#include <utility>

#include <microsoft/net/IpAddressInformation.hxx>
#include <microsoft/net/NetworkOperationsLinux.hxx>
#include <microsoft/net/netlink/route/NetlinkRoute.hxx>

namespace Microsoft::Net
{
namespace detail
{
NetworkInterfaceType
GetNetworkInterfaceType(const Netlink::NetlinkLink& netlinkLink) noexcept
{
    if (netlinkLink.Type == "wireless") {
        return NetworkInterfaceType::Wifi;
    }
    if (netlinkLink.Type == "ethernet" || netlinkLink.Type == "loopback" || netlinkLink.Type == "bridge") {
        return NetworkInterfaceType::Other;
    }
    return NetworkInterfaceType::Unknown;
}

IpFamily
GetIpFamily(const Netlink::NetlinkIpAddress& netlinkIpAddress) noexcept
{
    switch (netlinkIpAddress.Family) {
    case AF_INET:
        return IpFamily::Ipv4;
    case AF_INET6:
        return IpFamily::Ipv6;
    default:
        return IpFamily::Unknown;
    }
}

std::pair<std::string, IpAddressInformation>
NetlinkToIpAddressInformation(const Netlink::NetlinkIpAddress& netlinkIpAddress, const Netlink::NetlinkLink& netlinkLink) noexcept
{
    std::string ipAddress{ netlinkIpAddress.Address };
    const IpAddressInformation ipAddressInformation{
        .Family = GetIpFamily(netlinkIpAddress),
        .InterfaceType = GetNetworkInterfaceType(netlinkLink),
    };

    return { std::move(ipAddress), ipAddressInformation };
}

} // namespace detail

std::unordered_map<std::string, IpAddressInformation>
NetworkOperationsLinux::GetLocalIpAddressInformation(std::string_view ipAddress) const noexcept
{
    std::unordered_map<std::string, IpAddressInformation> results{};
    const auto netlinkIpAddresses = Netlink::NetlinkEnumerateIpAddresses();

    if (IsAnyAddress(ipAddress)) {
        for (const auto& netlinkIpAddress : netlinkIpAddresses) {
            auto netlinkLink = Netlink::NetlinkLink::FromInterfaceIndex(netlinkIpAddress.InterfaceIndex);
            if (netlinkLink.has_value()) {
                auto [ipAddressCopy, ipAddressInformation] = detail::NetlinkToIpAddressInformation(netlinkIpAddress, *netlinkLink);
                results.emplace(std::move(ipAddressCopy), ipAddressInformation);
            }
        }
    } else {
        const auto matchesIpAnyPort = [ipAddress](const Netlink::NetlinkIpAddress& netlinkIpAddress) {
            return ipAddress.starts_with(netlinkIpAddress.Address);
        };

        const auto netlinkIpAddress = std::ranges::find_if(netlinkIpAddresses, matchesIpAnyPort);
        if (netlinkIpAddress != std::ranges::end(netlinkIpAddresses)) {
            auto netlinkLink = Netlink::NetlinkLink::FromInterfaceIndex(netlinkIpAddress->InterfaceIndex);
            if (netlinkLink.has_value()) {
                auto [ipAddressCopy, ipAddressInformation] = detail::NetlinkToIpAddressInformation(*netlinkIpAddress, *netlinkLink);
                results.emplace(std::move(ipAddressCopy), ipAddressInformation);
            }
        }
    }

    return results;
}
} // namespace Microsoft::Net
