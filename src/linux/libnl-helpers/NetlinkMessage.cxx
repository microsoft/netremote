
#include <microsoft/net/netlink/NetlinkMessage.hxx>
#include <netlink/msg.h>

using namespace Microsoft::Net::Netlink;

NetlinkMessage::NetlinkMessage(struct nl_msg *message)
    : Message(message)
{
}

NetlinkMessage::~NetlinkMessage()
{
    if (Message != nullptr)
    {
        nlmsg_free(Message);
        Message = nullptr;
    }
}

NetlinkMessage::operator struct nl_msg *() const noexcept
{
    return Message;
}
