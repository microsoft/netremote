
#ifndef MICROSOFT_NET_NETLINK_NETLINK_SOCKET_HXX
#define MICROSOFT_NET_NETLINK_NETLINK_SOCKET_HXX

#include <netlink/netlink.h>

namespace Microsoft::Net::Netlink
{
struct NetlinkSocket
{
    struct nl_sock *Socket{ nullptr };

    NetlinkSocket(struct nl_sock *socket);
    ~NetlinkSocket();

    operator struct nl_sock *() const noexcept;
};
} // namespace Microsoft::Net::Netlink

#endif // MICROSOFT_NET_NETLINK_NETLINK_SOCKET_HXX
