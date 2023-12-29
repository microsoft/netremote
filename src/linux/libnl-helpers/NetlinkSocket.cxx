
#include <microsoft/net/netlink/NetlinkSocket.hxx>

using namespace Microsoft::Net::Netlink;

/* static */
NetlinkSocket
NetlinkSocket::Allocate()
{
    auto socket = nl_socket_alloc();
    return NetlinkSocket{ socket };
}

NetlinkSocket::NetlinkSocket(struct nl_sock *socket) :
    Socket(socket)
{
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

NetlinkSocket::operator struct nl_sock *() const noexcept
{
    return Socket;
}
