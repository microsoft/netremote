
#include <microsoft/net/netlink/NetlinkMessage.hxx>

using namespace Microsoft::Net::Netlink;

NetlinkMessage::NetlinkMessage(struct nl_msg* message) :
    Message(message)
{
}

NetlinkMessage::NetlinkMessage(NetlinkMessage&& other) :
    Message(other.Message)
{
    other.Message = nullptr;
}

NetlinkMessage&
NetlinkMessage::operator=(NetlinkMessage&& other)
{
    if (this != &other) {
        Reset();
        Message = other.Message;
        other.Message = nullptr;
    }

    return *this;
}

NetlinkMessage::~NetlinkMessage()
{
    Reset();
}

void
NetlinkMessage::Reset()
{
    if (Message != nullptr) {
        nlmsg_free(Message);
        Message = nullptr;
    }
}

struct nl_msg*
NetlinkMessage::Release() noexcept
{
    auto message = Message;
    Message = nullptr;
    return message;
}

NetlinkMessage::operator struct nl_msg *() const noexcept
{
    return Message;
}

struct nlmsghdr*
NetlinkMessage::Header() const noexcept
{
    return nlmsg_hdr(Message);
}
