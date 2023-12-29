
#ifndef MICROSOFT_NET_NETLINK_NETLINK_SOCKET_HXX
#define MICROSOFT_NET_NETLINK_NETLINK_SOCKET_HXX

#include <netlink/socket.h>
#include <netlink/netlink.h>

namespace Microsoft::Net::Netlink
{
/**
 * @brief Helper for managing a netlink socket, struct nl_sock. This class is
 * not thread-safe.
 */
struct NetlinkSocket
{
    /**
     * @brief The netlink socket owned by this object.
     */
    struct nl_sock* Socket{ nullptr };

    /**
     * @brief Allocate a new struct nl_sock, and wrap it in a NetlinkSocket.
     *
     * @return NetlinkSocket
     */
    static NetlinkSocket
    Allocate();

    /**
     * @brief Construct a default NetlinkSocket object that does not own a
     * netlink socket object.
     */
    NetlinkSocket() = default;

    /**
     * @brief Delete the copy constructor to enforce unique ownership.
     */
    NetlinkSocket(const NetlinkSocket&) = delete;

    /**
     * @brief Delete the copy assignment operator to enforce unique ownership.
     *
     * @return NetlinkSocket&
     */
    NetlinkSocket&
    operator=(const NetlinkSocket&) = delete;

    /**
     * @brief Move-construct a new NetlinkSocket object. This takes ownership of
     * the other instance.
     *
     * @param other The other instance to move from.
     */
    NetlinkSocket(NetlinkSocket&& other);

    /**
     * @brief Move-assign this instance from another instance. This takes
     * ownership of the other instance.
     *
     * @param other The other instance to move from.
     * @return NetlinkSocket&
     */
    NetlinkSocket&
    operator=(NetlinkSocket&& other);

    /**
     * @brief Construct a new NetlinkSocket that manages a pre-existing struct
     * nl_sock object. Note that once construction is complete, this object owns
     * the socket and will free it when it is destroyed.
     *
     * @param socket The netlink socket to manage.
     */
    NetlinkSocket(struct nl_sock* socket);

    /**
     * @brief Destroy the NetlinkSocket object, freeing the managed netlink
     * socket if it exists.
     */
    virtual ~NetlinkSocket();

    /**
     * @brief Reset the managed netlink socket, freeing it if it exists.
     */
    void
    Reset();

    /**
     * @brief Release ownership of the managed netlink socket, returning it to
     * the caller.
     */
    struct nl_sock*
    Release() noexcept;

    /**
     * @brief Implicit conversion operator to struct nl_sock *, allowing this
     * class to be used in netlink API calls.
     *
     * @return struct nl_sock * The netlink socket managed by this object.
     */
    operator struct nl_sock *() const noexcept;
};
} // namespace Microsoft::Net::Netlink

#endif // MICROSOFT_NET_NETLINK_NETLINK_SOCKET_HXX
