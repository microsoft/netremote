
#include <microsoft/net/netlink/NetlinkSocket.hxx>

using namespace Microsoft::Net::Netlink;

NetlinkSocket::NetlinkSocket(struct nl_sock *socket)
    : Socket(socket)
{
}

NetlinkSocket::~NetlinkSocket()
{
    if (Socket != nullptr)
    {
        nl_socket_free(Socket);
        Socket = nullptr;
    }
}

NetlinkSocket::operator struct nl_sock *() const noexcept
{
    return Socket;
}
