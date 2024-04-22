
#include <format>
#include <system_error>

#include <linux/netlink.h>
#include <microsoft/net/netlink/NetlinkErrorCategory.hxx>
#include <microsoft/net/netlink/NetlinkRoute.hxx>
#include <microsoft/net/netlink/NetlinkSocket.hxx>
#include <netlink/netlink.h>
#include <plog/Log.h>

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
} // namespace Microsoft::Net::Netlink
