
#include <system_error>

#include <microsoft/net/netlink/NetlinkErrorCategory.hxx>
#include <microsoft/net/netlink/NetlinkSocket.hxx>
#include <netlink/errno.h>
#include <netlink/handlers.h>
#include <netlink/socket.h>

using namespace Microsoft::Net::Netlink;

/* static */
NetlinkSocket
NetlinkSocket::Allocate()
{
    auto* socket = nl_socket_alloc();
    if (socket == nullptr) {
        throw std::system_error(MakeNetlinkErrorCode(NLE_NOMEM), "Failed to allocate netlink socket");
    }

    return NetlinkSocket{ socket };
}

NetlinkSocket::NetlinkSocket(struct nl_sock* socket) :
    Socket(socket)
{
}

NetlinkSocket::NetlinkSocket(NetlinkSocket&& other) noexcept :
    Socket(other.Socket)
{
    other.Socket = nullptr;
}

NetlinkSocket&
NetlinkSocket::operator=(NetlinkSocket&& other) noexcept
{
    if (this != &other) {
        Reset();
        Socket = other.Socket;
        other.Socket = nullptr;
    }

    return *this;
}

NetlinkSocket::~NetlinkSocket()
{
    Reset();
}

void
NetlinkSocket::Reset() noexcept
{
    if (Socket != nullptr) {
        nl_socket_free(Socket);
        Socket = nullptr;
    }
}

struct nl_sock*
NetlinkSocket::Release() noexcept
{
    auto* socket = Socket;
    Socket = nullptr;
    return socket;
}

NetlinkSocket::operator struct nl_sock *() const noexcept
{
    return Socket;
}
