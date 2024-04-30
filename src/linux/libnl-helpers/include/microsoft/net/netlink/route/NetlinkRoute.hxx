
#ifndef NETLINK_ROUTE_HXX
#define NETLINK_ROUTE_HXX

#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include <microsoft/net/netlink/NetlinkSocket.hxx>
#include <netlink/cache.h>
#include <netlink/route/addr.h>
#include <netlink/route/link.h>

namespace Microsoft::Net::Netlink
{
struct NetlinkIpAddress
{
    int InterfaceIndex;
    int Family;
    int PrefixLength;
    std::string Address;

    static NetlinkIpAddress
    FromRtnlAddr(struct rtnl_addr *addr);

    // static std::optional<NetlinkIpAddress>
    // FromAddress(std::string_view address, struct nl_cache *rtnlAddrCache = nullptr);

    auto
    operator<=>(const NetlinkIpAddress &) const = default;

    std::string
    ToString(bool showInterfaceIndex = true) const;
};

struct NetlinkLink
{
    int InterfaceIndex;
    std::string Name;
    std::string Type;
    std::string MacAddress;

    auto
    operator<=>(const NetlinkLink &) const = default;

    static NetlinkLink
    FromRtnlLink(struct rtnl_link *link);

    static std::optional<NetlinkLink>
    FromInterfaceIndex(int interfaceIndex);

    std::string
    ToString() const;
};

/**
 * @brief Create a netlink socket for use with the "route" family (NETLINK_ROUTE).
 *
 * This creates a netlink socket and connects it to the route netlink family.
 *
 * @return Microsoft::Net::Netlink::NetlinkSocket
 */
Microsoft::Net::Netlink::NetlinkSocket
CreateNlRouteSocket();

/**
 * @brief Enumerate all ip addresses on the system.
 *
 * @return std::unordered_set<NetlinkIpAddress>
 */
std::unordered_set<NetlinkIpAddress>
NetlinkEnumerateIpAddresses();

/**
 * @brief Enumerate all links (network interfaces) on the system.
 *
 * @return std::unordered_set<NetlinkLink>
 */
std::unordered_set<NetlinkLink>
NetlinkEnumerateLinks();

} // namespace Microsoft::Net::Netlink

namespace std
{
template <>
struct hash<Microsoft::Net::Netlink::NetlinkIpAddress>
{
    std::size_t
    operator()(const Microsoft::Net::Netlink::NetlinkIpAddress &address) const
    {
        return std::hash<int>{}(address.InterfaceIndex) ^ std::hash<int>{}(address.Family) ^ std::hash<int>{}(address.PrefixLength) ^ std::hash<std::string>{}(address.Address);
    }
};

template <>
struct hash<Microsoft::Net::Netlink::NetlinkLink>
{
    std::size_t
    operator()(const Microsoft::Net::Netlink::NetlinkLink &link) const
    {
        return std::hash<int>{}(link.InterfaceIndex) ^ std::hash<std::string>{}(link.Name) ^ std::hash<std::string>{}(link.Type) ^ std::hash<std::string>{}(link.MacAddress);
    }
};
} // namespace std

#endif // NETLINK_ROUTE_HXX
