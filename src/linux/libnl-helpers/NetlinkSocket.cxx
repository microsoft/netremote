
#include <microsoft/net/netlink/NetlinkSocket.hxx>

using namespace Microsoft::Net::Netlink;

/* static */
NetlinkSocket
NetlinkSocket::Allocate()
{
    auto socket = nl_socket_alloc();
    return NetlinkSocket{ socket };
}

NetlinkSocket::NetlinkSocket(struct nl_sock* socket) :
    Socket(socket)
{
}

NetlinkSocket::NetlinkSocket(NetlinkSocket&& other) :
    Socket(other.Socket)
{
    other.Socket = nullptr;
}

NetlinkSocket&
NetlinkSocket::operator=(NetlinkSocket&& other)
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
NetlinkSocket::Reset()
{
    if (Socket != nullptr) {
        nl_socket_free(Socket);
        Socket = nullptr;
    }
}

struct nl_sock*
NetlinkSocket::Release() noexcept
{
    auto socket = Socket;
    Socket = nullptr;
    return socket;
}

NetlinkSocket::operator struct nl_sock *() const noexcept
{
    return Socket;
}
