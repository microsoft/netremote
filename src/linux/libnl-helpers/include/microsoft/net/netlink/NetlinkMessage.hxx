
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
    struct nl_msg* Message{ nullptr };

    /**
     * @brief Allocate a new struct nl_msg, and wrap it in a NetlinkMessage.
     *
     * @return NetlinkMessage
     */
    static NetlinkMessage
    Allocate();

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
    explicit NetlinkMessage(struct nl_msg* message);

    /**
     * @brief Delete the copy constructor to enforce unique ownership.
     */
    NetlinkMessage(const NetlinkMessage&) = delete;

    /**
     * @brief Delete the copy assignment operator to enforce unique ownership.
     *
     * @return NetlinkMessage&
     */
    NetlinkMessage&
    operator=(const NetlinkMessage&) = delete;

    /**
     * @brief Move-construct a new NetlinkMessage object. This takes ownership of
     * the other instance.
     *
     * @param other The other instance to move from.
     */
    NetlinkMessage(NetlinkMessage&& other) noexcept;

    /**
     * @brief Move-assign this instance from another instance. This takes
     * ownership of the other instance.
     *
     * @param other The other instance to move from.
     * @return NetlinkMessage&
     */
    NetlinkMessage&
    operator=(NetlinkMessage&& other) noexcept;

    /**
     * @brief Destroy the NetlinkMessage object, freeing the managed netlink
     * message if it exists.
     */
    ~NetlinkMessage();

    /**
     * @brief Reset the managed netlink message, freeing it if it exists.
     */
    void
    Reset();

    /**
     * @brief Release ownership of the managed netlink message, returning it to
     * the caller.
     */
    struct nl_msg*
    Release() noexcept;

    /**
     * @brief Implicit conversion operator to struct nl_msg *, allowing this
     * class to be used in netlink API calls.
     *
     * @return struct nl_msg * The netlink message managed by this object.
     */
    operator struct nl_msg *() const noexcept;  // NOLINT(hicpp-explicit-conversions)

    /**
     * @brief Obtain the message header.
     *
     * @return struct nlmsghdr*
     */
    struct nlmsghdr*
    Header() const noexcept;
};
} // namespace Microsoft::Net::Netlink

#endif // MICROSOFT_NET_NETLINK_NETLINK_MESSAGE_HXX
