
#ifndef NETLINK_ROUTE_HXX
#define NETLINK_ROUTE_HXX

#include <string>
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

} // namespace Microsoft::Net::Netlink

#endif // NETLINK_ROUTE_HXX
