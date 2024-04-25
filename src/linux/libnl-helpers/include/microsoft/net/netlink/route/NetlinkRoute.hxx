
#ifndef NETLINK_ROUTE_HXX
#define NETLINK_ROUTE_HXX

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <microsoft/net/netlink/NetlinkSocket.hxx>

namespace Microsoft::Net::Netlink
{
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
 * @brief Enumerate all ipv4 addresses on the system.
 * 
 * @return std::vector<std::string> 
 */
std::vector<std::string>
NetlinkEnumerateIpv4Addresses();

struct NetlinkInterfaceLinkInfo
{
    std::string LinkType;
    std::string IpAddress;
};

/**
 * @brief Enumerate information about network interfaces on the system.
 * 
 * @return std::unordered_map<std::string, NetlinkInterfaceLinkInfo> 
 */
std::unordered_map<std::string, NetlinkInterfaceLinkInfo>
NetlinkEnumerateInterfaceInfo();

} // namespace Microsoft::Net::Netlink

#endif // NETLINK_ROUTE_HXX
