
#ifndef MICROSOFT_NET_NETLINK_NETLINK_MESSAGE_HXX
#define MICROSOFT_NET_NETLINK_NETLINK_MESSAGE_HXX

#include <netlink/msg.h>
#include <netlink/netlink.h>

namespace Microsoft::Net::Netlink
{
/**
 * @brief Heler for managing a netlink message, struct nl_msg. This class is not
 * thread-safe.
 */
struct NetlinkMessage
{
    /**
     * @brief The netlink message owned by this object.
     */
    struct nl_msg *Message{ nullptr };

    /**
     * @brief Construct a new NetlinkMessage object that does not own a netlink
     * message object.
     */
    NetlinkMessage() = default;

    /**
     * @brief Construct a new NetlinkMessage object that manages a pre-existing
     * struct nl_msg object. Note that once construction is complete, this
     * object owns the message and will free it when it is destroyed.
     *
     * @param message The netlink message to manage.
     */
    NetlinkMessage(struct nl_msg *message);

    /**
     * @brief Destroy the NetlinkMessage object, freeing the managed netlink
     * message if it exists.
     */
    ~NetlinkMessage();

    /**
     * @brief Implicit conversion operator to struct nl_msg *, allowing this
     * class to be used in netlink API calls.
     *
     * @return struct nl_msg * The netlink message managed by this object.
     */
    operator struct nl_msg *() const noexcept;

    /**
     * @brief Obtain the message header.
     *
     * @return struct nlmsghdr*
     */
    struct nlmsghdr *
    Header() const noexcept;
};
} // namespace Microsoft::Net::Netlink

#endif // MICROSOFT_NET_NETLINK_NETLINK_MESSAGE_HXX
