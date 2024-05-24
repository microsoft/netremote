
#include <algorithm>
#include <format>
#include <ranges>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include <microsoft/net/IpAddressInformation.hxx>
#include <microsoft/net/NetworkOperationsLinux.hxx>
#include <microsoft/net/netlink/route/NetlinkRoute.hxx>
#include <net/if.h>
#include <plog/Log.h>
#include <sys/socket.h>

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
    if (netlinkLink.Type == "bridge") {
        return NetworkInterfaceType::Bridge;
    }
    if (netlinkLink.Type == "ethernet" || netlinkLink.Type == "loopback") {
        return NetworkInterfaceType::Other;
    }
    return NetworkInterfaceType::Unknown;
}

NetworkIpFamily
GetNetworkIpFamily(const Netlink::NetlinkIpAddress& netlinkIpAddress) noexcept
{
    switch (netlinkIpAddress.Family) {
    case AF_INET:
        return NetworkIpFamily::Ipv4;
    case AF_INET6:
        return NetworkIpFamily::Ipv6;
    default:
        return NetworkIpFamily::Unknown;
    }
}

std::pair<std::string, IpAddressInformation>
NetlinkToIpAddressInformation(const Netlink::NetlinkIpAddress& netlinkIpAddress, const Netlink::NetlinkLink& netlinkLink) noexcept
{
    std::string ipAddress{ netlinkIpAddress.Address };
    const IpAddressInformation ipAddressInformation{
        .Family = GetNetworkIpFamily(netlinkIpAddress),
        .InterfaceType = GetNetworkInterfaceType(netlinkLink),
        .InterfaceName = netlinkLink.Name,
    };

    return { std::move(ipAddress), ipAddressInformation };
}

/**
 * @brief Convert a NetlinkIpAddress to a NetworkIpAddress.
 *
 * @param netlinkIpAddress The NetlinkIpAddress to convert.
 * @return NetworkIpAddress The converted NetworkIpAddress.
 */
NetworkIpAddress
NetlinkIpAddressToNetworkIpAddress(const Netlink::NetlinkIpAddress& netlinkIpAddress) noexcept
{
    return NetworkIpAddress{
        .Family = GetNetworkIpFamily(netlinkIpAddress),
        .Address = netlinkIpAddress.Address,
        .PrefixLength = static_cast<uint32_t>(netlinkIpAddress.PrefixLength),
    };
}

/**
 * @brief Convert a NetlinkLink to a NetworkInterfaceId.
 *
 * @param netlinkLink The NetlinkLink to convert.
 * @return NetworkInterfaceId The converted NetworkInterfaceId.
 */
NetworkInterfaceId
NetlinkLinkToNetworkInterfaceId(const Netlink::NetlinkLink& netlinkLink) noexcept
{
    return NetworkInterfaceId{
        .Name = netlinkLink.Name,
        .Type = GetNetworkInterfaceType(netlinkLink),
    };
}
} // namespace detail

std::unordered_set<NetworkInterfaceId>
NetworkOperationsLinux::EnumerateNetworkInterfaces() const noexcept
{
    std::unordered_set<NetworkInterfaceId> networkInterfaces{};
    const auto netlinkLinks = Netlink::NetlinkEnumerateLinks();

    for (auto networkInterfaceId : netlinkLinks | std::views::transform(detail::NetlinkLinkToNetworkInterfaceId)) {
        networkInterfaces.insert(std::move(networkInterfaceId));
    }

    return networkInterfaces;
}

std::unordered_set<NetworkIpAddress>
NetworkOperationsLinux::GetNetworkInterfaceAddresses(std::string_view networkInterfaceName) const noexcept
{
    unsigned int networkInterfaceIndex = if_nametoindex(std::data(networkInterfaceName));
    if (networkInterfaceIndex == 0) {
        LOGE << std::format("Failed to get interface index for interface {}", networkInterfaceName);
        return {};
    }

    // This lambda is used to filter out IP addresses that do not match the interface index.
    const auto matchesInterface = [&networkInterfaceIndex](const Netlink::NetlinkIpAddress& netlinkIpAddress) {
        return netlinkIpAddress.InterfaceIndex == static_cast<int>(networkInterfaceIndex);
    };

    // This enumerates all IP addresses, but we only care about the ones that match the interface. We should really
    // enumerate only those that match the interface name/index.
    const auto netlinkIpAddresses = Netlink::NetlinkEnumerateIpAddresses();

    std::unordered_set<NetworkIpAddress> networkIpAddresses{};
    for (auto networkIpAddress : netlinkIpAddresses | std::views::filter(matchesInterface) | std::views::transform(detail::NetlinkIpAddressToNetworkIpAddress)) {
        networkIpAddresses.insert(std::move(networkIpAddress));
    }

    return networkIpAddresses;
}

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
