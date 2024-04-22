
#ifndef NETLINK_ROUTE_HXX
#define NETLINK_ROUTE_HXX

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

} // namespace Microsoft::Net::Netlink

#endif // NETLINK_ROUTE_HXX
