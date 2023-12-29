
#ifndef MICROSOFT_NET_NETLINK_NETLINK_MESSAGE_HXX
#define MICROSOFT_NET_NETLINK_NETLINK_MESSAGE_HXX

#include <netlink/netlink.h>

namespace Microsoft::Net::Netlink
{
struct NetlinkMessage
{
    struct nl_msg *Message{ nullptr };

    NetlinkMessage(struct nl_msg *message);
    ~NetlinkMessage();

    operator struct nl_msg *() const noexcept;
};
} // namespace Microsoft::Net::Netlink

#endif // MICROSOFT_NET_NETLINK_NETLINK_MESSAGE_HXX
