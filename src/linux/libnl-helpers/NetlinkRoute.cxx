
#include <array>
#include <cstdint>
#include <cstring>
#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <unordered_set>
#include <utility>
#include <vector>

#include <linux/netlink.h>
#include <microsoft/net/netlink/NetlinkErrorCategory.hxx>
#include <microsoft/net/netlink/NetlinkSocket.hxx>
#include <microsoft/net/netlink/route/NetlinkRoute.hxx>
#include <net/if.h>
#include <netinet/in.h>
#include <netlink/addr.h>
#include <netlink/cache.h>
#include <netlink/netlink.h>
#include <netlink/object.h>
#include <netlink/route/addr.h>
#include <netlink/route/link.h>
#include <notstd/Scope.hxx>
#include <plog/Log.h>
#include <sys/socket.h>

namespace Microsoft::Net::Netlink
{
namespace detail
{
// The maximum length of an ipv4 address is INET_ADDRSTRLEN (16) + 3 (for the subnet mask), eg. xxx.xxx.xxx.xxx/yy.
constexpr auto Ipv4AddressAsciiLengthMax{ INET_ADDRSTRLEN + 3 };
constexpr auto Ipv6AddressAsciiLengthMax{ INET6_ADDRSTRLEN + 5 };

/**
 * @brief Get the (maximum) length of an address in ascii format.
 *
 * @param addressFamily The address family to get the length for. This must be either AF_INET or AF_INET6.
 * @return std::size_t The maximum length of an address string corresponding to the specified IP family.
 */
std::size_t
GetAddressAsciiLength(int addressFamily) noexcept
{
    switch (addressFamily) {
    case AF_INET:
        return Ipv4AddressAsciiLengthMax;
    case AF_INET6:
        [[fallthrough]];
    default:
        return Ipv6AddressAsciiLengthMax;
    }
}

/**
 * @brief Get a string representation of the specified IP address family.
 *
 * @param addressFamily The address family to get the name for. This must be either AF_INET or AF_INET6.
 * @return constexpr std::string_view The name of the address family or "Unknown" if the address family is not
 * recognized.
 */
constexpr std::string_view
GetAddressFamilyName(int addressFamily) noexcept
{
    switch (addressFamily) {
    case AF_INET:
        return "IPv4";
    case AF_INET6:
        return "IPv6";
    default:
        return "Unknown";
    }
}

/**
 * @brief Helper function to use with nl_cache_foreach when processing an rtnl link cache. This will parse the netlink
 * object and if it represents a valid netlink link, add an entry to the map specified by the context.
 *
 * @param nlObjectLink The netlink object representing a potential netlink link.
 * @param context The context to populate with the result. This must be of type std::unordered_set<NetlinkLink>.
 */
void
OnLink(struct nl_object *nlObjectLink, void *context)
{
    auto *rtnlLink = reinterpret_cast<struct rtnl_link *>(nlObjectLink); // NOLINT
    auto netlinkLink = NetlinkLink::FromRtnlLink(rtnlLink);

    // Populate output variable (context) with result.
    auto &netlinkLinks = *static_cast<std::unordered_set<NetlinkLink> *>(context);
    netlinkLinks.emplace(std::move(netlinkLink));
}

/**
 * @brief Helper function to use with nl_cache_foreach when processing an rtnl address cache. This parses the specified
 * netlink object and if it represents a valid netlink address, add an entry to the map specified by the context.
 *
 * @param nlObjectAddress The netlink object representing a potential netlink address.
 * @param context The context to populate with the result. This must be of type std::unordered_set<NetlinkIpAddress>.
 */
void
OnAddress(struct nl_object *nlObjectAddress, void *context)
{
    auto *rtnlAddress = reinterpret_cast<struct rtnl_addr *>(nlObjectAddress); // NOLINT
    auto netlinkAddress = NetlinkIpAddress::FromRtnlAddr(rtnlAddress);

    // Populate output variable (context) with result.
    auto &netlinkAddresses = *static_cast<std::unordered_set<NetlinkIpAddress> *>(context);
    netlinkAddresses.emplace(std::move(netlinkAddress));
}
} // namespace detail

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

std::unordered_set<NetlinkLink>
NetlinkEnumerateLinks()
{
    auto nlRouteSocket{ CreateNlRouteSocket() };

    struct nl_cache *linkCache{ nullptr };
    const int ret = rtnl_link_alloc_cache(nlRouteSocket, AF_UNSPEC, &linkCache);
    if (ret != 0) {
        const auto errorCode = MakeNetlinkErrorCode(-ret);
        const auto message = std::format("Failed to allocate link cache with error {}", errorCode.value());
        LOGE << message;
        throw std::system_error(errorCode, message);
    }

    auto freeLinkCache = notstd::scope_exit([&linkCache] {
        nl_cache_free(linkCache);
    });

    std::unordered_set<NetlinkLink> links{};
    nl_cache_foreach(linkCache, detail::OnLink, &links);

    return links;
}

std::unordered_set<NetlinkIpAddress>
NetlinkEnumerateIpAddresses()
{
    auto nlRouteSocket{ CreateNlRouteSocket() };

    struct nl_cache *ipAddressCache{ nullptr };
    const int ret = rtnl_addr_alloc_cache(nlRouteSocket, &ipAddressCache);
    if (ret != 0) {
        const auto errorCode = MakeNetlinkErrorCode(-ret);
        const auto message = std::format("Failed to allocate address cache with error {}", errorCode.value());
        LOGE << message;
        throw std::system_error(errorCode, message);
    }

    auto freeIpAddressCache = notstd::scope_exit([&ipAddressCache] {
        nl_cache_free(ipAddressCache);
    });

    std::unordered_set<NetlinkIpAddress> ipAddresses{};
    nl_cache_foreach(ipAddressCache, detail::OnAddress, &ipAddresses);

    return ipAddresses;
}

/* static */
NetlinkIpAddress
NetlinkIpAddress::FromRtnlAddr(struct rtnl_addr *rtnlAddress)
{
    const auto index = rtnl_addr_get_ifindex(rtnlAddress);
    const auto family = rtnl_addr_get_family(rtnlAddress);
    const auto prefixLength = rtnl_addr_get_prefixlen(rtnlAddress);
    const auto *local = rtnl_addr_get_local(rtnlAddress);

    // Convert the address to a string then resize to actual null-terminated length.
    std::string addressLocalAscii(detail::GetAddressAsciiLength(family), '\0');
    nl_addr2str(local, std::data(addressLocalAscii), std::size(addressLocalAscii));
    addressLocalAscii.resize(std::strlen(std::data(addressLocalAscii)));

    return NetlinkIpAddress{ index, family, prefixLength, addressLocalAscii };
}

/* static */
NetlinkLink
NetlinkLink::FromRtnlLink(struct rtnl_link *rtnlLink)
{
    const auto index = rtnl_link_get_ifindex(rtnlLink);
    const auto *name = rtnl_link_get_name(rtnlLink);
    const auto *type = rtnl_link_get_type(rtnlLink);
    const auto *macAddress = rtnl_link_get_addr(rtnlLink);

    // TODO: implement independent function to get proper type when this is nullptr.
    if (type == nullptr) {
        type = "none";
    }

    // Convert the address to a string then resize to actual null-terminated length.
    std::string macAddressAscii(detail::Ipv6AddressAsciiLengthMax, '\0');
    nl_addr2str(macAddress, std::data(macAddressAscii), std::size(macAddressAscii));
    macAddressAscii.resize(std::strlen(std::data(macAddressAscii)));

    return NetlinkLink{ index, name, type, std::move(macAddressAscii) };
}

/* static */
std::optional<NetlinkLink>
NetlinkLink::FromInterfaceIndex(int interfaceIndex)
{
    // Obtain interface/link name from interface index.
    std::array<char, IF_NAMESIZE> interfaceName{ '\0' };
    if_indextoname(static_cast<uint32_t>(interfaceIndex), std::data(interfaceName));

    // Allocate a netlink socket to make the kernel request with.
    auto nlRouteSocket{ CreateNlRouteSocket() };

    // Send a kernel request to get the link information.
    struct rtnl_link *rtnlLink{ nullptr };
    const int ret = rtnl_link_get_kernel(nlRouteSocket, interfaceIndex, std::data(interfaceName), &rtnlLink);
    if (ret < 0) {
        const auto errorCode = MakeNetlinkErrorCode(-ret);
        const auto message = std::format("Failed to get link with error {}", errorCode.value());
        LOGE << message;
        return std::nullopt;
    }

    auto freeLink = notstd::scope_exit([&rtnlLink] {
        nl_object_free(OBJ_CAST(rtnlLink));
        rtnlLink = nullptr;
    });

    return FromRtnlLink(rtnlLink);
}

std::string
NetlinkLink::ToString() const
{
    return std::format("[{}] {} {} {}", InterfaceIndex, Name, Type, MacAddress);
}

std::string
NetlinkIpAddress::ToString(bool showInterfaceIndex) const
{
    auto addressFamilyName = detail::GetAddressFamilyName(Family);

    return showInterfaceIndex
        ? std::format("[{}] {} {}/{}", InterfaceIndex, addressFamilyName, Address, PrefixLength)
        : std::format("{} {}/{}", addressFamilyName, Address, PrefixLength);
}

} // namespace Microsoft::Net::Netlink
