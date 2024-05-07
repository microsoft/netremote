
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
/**
 * @brief Represents IP address information obtained from a netlink address (struct rtnl_addr) object.
 */
struct NetlinkIpAddress
{
    int InterfaceIndex;
    int Family;
    int PrefixLength;
    std::string Address;

    auto
    operator<=>(const NetlinkIpAddress &) const = default;

    /**
     * @brief Parse a netlink address object and create a NetlinkIpAddress object from it.
     *
     * @param rtnlAddress The netlink address object to parse.
     * @return NetlinkIpAddress
     */
    static NetlinkIpAddress
    FromRtnlAddr(struct rtnl_addr *rtnlAddress);

    /**
     * @brief Get a string representation of the IP address.
     *
     * @param showInterfaceIndex Whether to include the interface index in the string representation.
     * @return std::string
     */
    std::string
    ToString(bool showInterfaceIndex = true) const;
};

/**
 * @brief Represents link information obtained from a netlink link (struct rtnl_link) object. A link is mostly
 * synonymous with a network interface.
 */
struct NetlinkLink
{
    int InterfaceIndex;
    std::string Name;
    std::string Type;
    std::string MacAddress;

    auto
    operator<=>(const NetlinkLink &) const = default;

    /**
     * @brief Parse a netlink link object and create a NetlinkLink object from it.
     *
     * @param link The netlink link object to parse.
     * @return NetlinkLink
     */
    static NetlinkLink
    FromRtnlLink(struct rtnl_link *link);

    /**
     * @brief Get a NetlinkLink object from an interface index.
     *
     * @param interfaceIndex
     * @return std::optional<NetlinkLink>
     */
    static std::optional<NetlinkLink>
    FromInterfaceIndex(int interfaceIndex);

    /**
     * @brief Get a string representation of the link.
     *
     * @return std::string
     */
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
