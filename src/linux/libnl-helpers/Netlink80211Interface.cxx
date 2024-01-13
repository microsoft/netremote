
#include <array>
#include <format>

#include <linux/genetlink.h>
#include <magic_enum.hpp>
#include <microsoft/net/netlink/NetlinkMessage.hxx>
#include <microsoft/net/netlink/NetlinkSocket.hxx>
#include <microsoft/net/netlink/nl80211/Netlink80211.hxx>
#include <microsoft/net/netlink/nl80211/Netlink80211Interface.hxx>
#include <microsoft/net/netlink/nl80211/Netlink80211ProtocolState.hxx>
#include <netlink/genl/genl.h>
#include <plog/Log.h>

using namespace Microsoft::Net::Netlink::Nl80211;

using Microsoft::Net::Netlink::NetlinkMessage;
using Microsoft::Net::Netlink::NetlinkSocket;

Nl80211Interface::Nl80211Interface(std::string_view name, nl80211_iftype type, uint32_t index, uint32_t wiphyIndex) noexcept :
    Name(name),
    Type(type),
    Index(index),
    WiphyIndex(wiphyIndex)
{
}

std::string
Nl80211Interface::ToString() const
{
    return std::format("[{}/{}] {} {}", Index, WiphyIndex, Name, magic_enum::enum_name(Type));
}

/* static */
std::optional<Nl80211Interface>
Nl80211Interface::Parse(struct nl_msg *nl80211Message) noexcept
{
    // Ensure the message is valid.
    if (nl80211Message == nullptr) {
        LOGE << "Received null nl80211 message";
        return std::nullopt;
    }

    // Ensure the message has a valid genl header.
    auto *nl80211MessageHeader{ static_cast<struct nlmsghdr *>(nlmsg_hdr(nl80211Message)) };
    if (genlmsg_valid_hdr(nl80211MessageHeader, 1) < 0) {
        LOGE << "Received invalid nl80211 message header";
        return std::nullopt;
    }

    // Extract the nl80211 (genl) message header.
    const auto *genl80211MessageHeader{ static_cast<struct genlmsghdr *>(nlmsg_data(nl80211MessageHeader)) };

    // Parse the message.
    std::array<struct nlattr *, NL80211_ATTR_MAX + 1> newInterfaceMessageAttributes{};
    int ret = nla_parse(std::data(newInterfaceMessageAttributes), std::size(newInterfaceMessageAttributes), genlmsg_attrdata(genl80211MessageHeader, 0), genlmsg_attrlen(genl80211MessageHeader, 0), nullptr);
    if (ret < 0) {
        LOG_ERROR << std::format("Failed to parse netlink message attributes with error {} ({})", ret, strerror(-ret));
        return std::nullopt;
    }

    // Tease out parameters to populate the Nl80211Interface instance.
    auto *interfaceName = static_cast<const char *>(nla_data(newInterfaceMessageAttributes[NL80211_ATTR_IFNAME]));
    auto interfaceType = static_cast<nl80211_iftype>(nla_get_u32(newInterfaceMessageAttributes[NL80211_ATTR_IFTYPE]));
    auto interfaceIndex = static_cast<uint32_t>(nla_get_u32(newInterfaceMessageAttributes[NL80211_ATTR_IFINDEX]));
    auto wiphyIndex = static_cast<uint32_t>(nla_get_u32(newInterfaceMessageAttributes[NL80211_ATTR_WIPHY]));

    return Nl80211Interface(interfaceName, interfaceType, interfaceIndex, wiphyIndex);
}

namespace detail
{
/**
 * @brief Handler function for NL80211_CMD_GET_INTERFACE responses.
 *
 * @param nl80211Message The response message to a NL80211_CMD_GET_INTERFACE dump request.
 * @param context The context pointer provided to nl_socket_modify_cb. This must be a std::vector<Nl80211Interface>*.
 * @return int
 */
int
HandleNl80211InterfaceDumpResponse(struct nl_msg *nl80211Message, void *context)
{
    if (context == nullptr) {
        LOGE << "Received nl80211 interface dump response with null context";
        return NL_SKIP;
    }

    // Extract vector to populate with interfaces.
    auto &nl80211Interfaces = *static_cast<std::vector<Nl80211Interface> *>(context);

    // Attempt to parse the message.
    auto nl80211Interface = Nl80211Interface::Parse(nl80211Message);
    if (!nl80211Interface.has_value()) {
        LOGW << "Failed to parse nl80211 interface dump response";
        return NL_SKIP;
    } else {
        LOGD << std::format("Parsed nl80211 interface dump response: {}", nl80211Interface->ToString());
    }

    nl80211Interfaces.push_back(std::move(nl80211Interface.value()));

    return NL_OK;
}
} // namespace detail

/* static */
std::vector<Nl80211Interface>
Nl80211Interface::Enumerate()
{
    // Allocate a new netlink socket.
    auto nl80211SocketOpt{ CreateNl80211Socket() };
    if (!nl80211SocketOpt.has_value()) {
        LOGE << "Failed to create nl80211 socket";
        return {};
    }

    // Allocate a new nl80211 message for sending the dump request for all interfaces.
    auto nl80211Socket{ std::move(nl80211SocketOpt.value()) };
    auto nl80211MessageGetInterfaces{ NetlinkMessage::Allocate() };
    if (nl80211MessageGetInterfaces == nullptr) {
        LOGE << "Failed to allocate nl80211 message for interface dump request";
        return {};
    }

    // Populate the genl message for the interface dump request.
    const int nl80211DriverId = Nl80211ProtocolState::Instance().DriverId;
    const auto *genlMessageGetInterfaces = genlmsg_put(nl80211MessageGetInterfaces, NL_AUTO_PID, NL_AUTO_SEQ, nl80211DriverId, 0, NLM_F_DUMP, NL80211_CMD_GET_INTERFACE, 0);
    if (genlMessageGetInterfaces == nullptr) {
        LOGE << "Failed to populate genl message for interface dump request";
        return {};
    }

    // Modify the socket callback to handle the response, providing a pointer to the vector to populate with interfaces.
    std::vector<Nl80211Interface> nl80211Interfaces{};
    int ret = nl_socket_modify_cb(nl80211Socket, NL_CB_VALID, NL_CB_CUSTOM, detail::HandleNl80211InterfaceDumpResponse, &nl80211Interfaces);
    if (ret < 0) {
        LOGE << std::format("Failed to modify socket callback with error {} ({})", ret, nl_geterror(ret));
        return {};
    }

    // Send the request.
    ret = nl_send_auto(nl80211Socket, nl80211MessageGetInterfaces);
    if (ret < 0) {
        LOGE << std::format("Failed to send interface dump request with error {} ({})", ret, nl_geterror(ret));
        return {};
    }

    // Receive the response, which will invoke the configured callback for each message.
    ret = nl_recvmsgs_default(nl80211Socket);
    if (ret < 0) {
        LOGE << std::format("Failed to receive interface dump response with error {} ({})", ret, nl_geterror(ret));
        return {};
    }

    return nl80211Interfaces;
}

std::optional<Nl80211Wiphy>
Nl80211Interface::GetWiphy() const
{
    return Nl80211Wiphy::FromIndex(WiphyIndex); 
}
