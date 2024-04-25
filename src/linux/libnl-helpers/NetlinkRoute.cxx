
#include <cstring>
#include <format>
#include <optional>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include <iostream> // TODO: remove me

#include <linux/netlink.h>
#include <microsoft/net/netlink/NetlinkErrorCategory.hxx>
#include <microsoft/net/netlink/NetlinkSocket.hxx>
#include <microsoft/net/netlink/route/NetlinkRoute.hxx>
#include <netinet/in.h>
#include <netlink/addr.h>
#include <netlink/cache.h>
#include <netlink/errno.h>
#include <netlink/netlink.h>
#include <netlink/object.h>
#include <netlink/route/addr.h>
#include <notstd/Scope.hxx>
#include <plog/Log.h>
#include <sys/socket.h>

namespace Microsoft::Net::Netlink
{
NetlinkSocket
CreateNlRouteSocket()
{
    // Allocate a new netlink socket.
    auto socket = NetlinkSocket::Allocate();

    // Connect the socket to the route netlink family.
    const int ret = nl_connect(socket, NETLINK_ROUTE);
    if (ret < 0) {
        const auto errorCode = MakeNetlinkErrorCode(-ret);
        const auto message = std::format("Failed to connect netlink socket for nl control with error {}", errorCode.value());
        LOGE << message;
        throw std::system_error(errorCode, message);
    }

    return socket;
}

namespace detail
{
// The maximum length of an ipv4 address is INET_ADDRSTRLEN (16) + 3 (for the subnet mask), eg. xxx.xxx.xxx.xxx/yy.
constexpr auto Ipv4AddressLengthMax{ INET_ADDRSTRLEN + 3 };
constexpr auto Ipv6AddressLengthMax{ INET6_ADDRSTRLEN + 5 };

void
OnLink(struct nl_object *nlObjectLink, void *context)
{
    auto *nlLink = reinterpret_cast<struct rtnl_link *>(nlObjectLink); // NOLINT
    auto &interfaceLinkInfo = *static_cast<std::unordered_map<std::string, NetlinkInterfaceLinkInfo> *>(context);

    const auto *linkType = rtnl_link_get_type(nlLink);
    const auto *linkName = rtnl_link_get_name(nlLink);
    const auto *linkIpAddress = rtnl_link_get_addr(nlLink);
    if (linkType == nullptr) {
        linkType = "none";
    }

    // Convert the IP address to a string.
    std::string linkIpAddressAscii(Ipv6AddressLengthMax, '\0');
    nl_addr2str(linkIpAddress, std::data(linkIpAddressAscii), std::size(linkIpAddressAscii));

    interfaceLinkInfo[linkName] = NetlinkInterfaceLinkInfo{ linkType, linkIpAddressAscii };
    std::cout << std::format("{}: {} {}", linkName, linkType, linkIpAddressAscii);
}
} // namespace detail

std::unordered_map<std::string, NetlinkInterfaceLinkInfo>
NetlinkEnumerateInterfaceInfo()
{
    auto nlRouteSocket{ CreateNlRouteSocket() };

    struct nl_cache *linkCache{ nullptr };
    int ret = rtnl_link_alloc_cache(nlRouteSocket, AF_UNSPEC, &linkCache);
    if (ret != 0) {
        const auto errorCode = MakeNetlinkErrorCode(-ret);
        const auto message = std::format("Failed to allocate link cache with error {}", errorCode.value());
        LOGE << message;
        throw std::system_error(errorCode, message);
    }

    auto freeLinkCache = notstd::scope_exit([&linkCache] {
        nl_cache_free(linkCache);
    });

    std::unordered_map<std::string, NetlinkInterfaceLinkInfo> interfaceLinkInfo{};
    nl_cache_foreach(linkCache, detail::OnLink, &interfaceLinkInfo);

    // int ret = rtnl_addr_alloc_cache(nlRouteSocket, &ipAddressCache);
    // if (ret != 0) {
    //     const auto errorCode = MakeNetlinkErrorCode(-ret);
    //     const auto message = std::format("Failed to allocate address cache with error {}", errorCode.value());
    //     LOGE << message;
    //     throw std::system_error(errorCode, message);
    // }

    return interfaceLinkInfo;
}

std::vector<std::string>
NetlinkEnumerateIpv4Addresses()
{
    auto nlRouteSocket{ CreateNlRouteSocket() };

    struct nl_cache *ipAddressCache{ nullptr };
    int ret = rtnl_addr_alloc_cache(nlRouteSocket, &ipAddressCache);
    if (ret != 0) {
        const auto errorCode = MakeNetlinkErrorCode(-ret);
        const auto message = std::format("Failed to allocate address cache with error {}", errorCode.value());
        LOGE << message;
        throw std::system_error(errorCode, message);
    }

    auto freeNlCache = notstd::scope_exit([&ipAddressCache] {
        nl_cache_free(ipAddressCache);
    });

    std::vector<std::string> ipv4Addresses{};
    struct nl_object *nlObjectIpAddress{ nullptr };
    for (nlObjectIpAddress = nl_cache_get_first(ipAddressCache); nlObjectIpAddress != nullptr; nlObjectIpAddress = nl_cache_get_next(nlObjectIpAddress)) {
        auto *nlIpAddress = reinterpret_cast<struct rtnl_addr *>(nlObjectIpAddress); // NOLINT

        // Process ipv4 addresses only.
        const auto family = rtnl_addr_get_family(nlIpAddress);
        if (family != AF_INET) {
            continue;
        }

        // Convert the ipv4 address to a string.
        auto *ipv4Address = rtnl_addr_get_local(nlIpAddress);
        std::string ipv4AddressAscii(detail::Ipv4AddressLengthMax, '\0');
        nl_addr2str(ipv4Address, std::data(ipv4AddressAscii), std::size(ipv4AddressAscii));

        {
            auto *nlLink = rtnl_addr_get_link(nlIpAddress);
            if (nlLink != nullptr) {
                const auto *linkType = rtnl_link_get_type(nlLink);
                std::cout << std::format("{}: {}", linkType, ipv4AddressAscii);
            } else {
                std::cout << std::format("nolink: {}", ipv4AddressAscii);
            }
        }

        // Resize to the actual null-terminated string length.
        ipv4AddressAscii.resize(std::strlen(std::data(ipv4AddressAscii)));
        ipv4Addresses.push_back(std::move(ipv4AddressAscii));
    }

    return ipv4Addresses;
}

} // namespace Microsoft::Net::Netlink
