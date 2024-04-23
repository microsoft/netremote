
#include <format>
#include <optional>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include <arpa/inet.h>
#include <linux/if_addr.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <microsoft/net/netlink/NetlinkErrorCategory.hxx>
#include <microsoft/net/netlink/NetlinkMessage.hxx>
#include <microsoft/net/netlink/NetlinkSocket.hxx>
#include <microsoft/net/netlink/route/NetlinkRoute.hxx>
#include <netinet/in.h>
#include <netlink/attr.h>
#include <netlink/errno.h>
#include <netlink/handlers.h>
#include <netlink/msg.h>
#include <netlink/netlink.h>
#include <netlink/object.h>
#include <netlink/route/addr.h>
#include <netlink/socket.h>
#include <plog/Log.h>
#include <sys/socket.h>
#include <unistd.h>

namespace detail
{
std::optional<std::string>
ParseNlGetAddressMessage([[maybe_unused]] struct nlmsghdr *nlMessageHeader)
{
    auto ifAddressMessage = static_cast<struct ifaddrmsg *>(nlmsg_data(nlMessageHeader));
    auto ifAddressMessageLength = IFA_PAYLOAD(nlMessageHeader);
    auto ifAddressAttribute = static_cast<struct rtattr *>(IFA_RTA(ifAddressMessage));

    for (;;) {
        if (!RTA_OK(ifAddressAttribute, ifAddressMessageLength)) {
            break;
        }

        switch (ifAddressAttribute->rta_type) {
        case IFA_LOCAL: {
            auto ipAddress = *reinterpret_cast<struct in_addr *>(RTA_DATA(ifAddressAttribute));
            auto *ipAddressString = inet_ntop(AF_INET, &ipAddress, nullptr, 0);
            if (ipAddressString != nullptr) {
                return ipAddressString;
            }
            break;
        }
        }

        ifAddressAttribute = RTA_NEXT(ifAddressAttribute, ifAddressMessageLength);
    }

    return {};
}

std::vector<std::string>
ParseNlGetAddressDumpResponse(struct nl_msg *nlMessage) noexcept
{
    std::vector<std::string> ipAddresses{};
    struct nlmsghdr *nlMessageHeader = nlmsg_hdr(nlMessage);
    auto nlMessageLength = nlMessageHeader->nlmsg_len;

    for (;;) {
        if (!NLMSG_OK(nlMessageHeader, nlMessageLength)) {
            break;
        }
        if (nlMessageHeader->nlmsg_type == NLMSG_DONE) {
            break;
        }
        if (nlMessageHeader->nlmsg_type == RTM_NEWADDR) {
            auto ipAddress = ParseNlGetAddressMessage(nlMessageHeader);
            if (ipAddress.has_value()) {
                ipAddresses.push_back(ipAddress.value());
            }
        }

        nlMessageHeader = NLMSG_NEXT(nlMessageHeader, nlMessageLength);
    }

    return ipAddresses;
}

int
HandleNlGetAddressResponse([[maybe_unused]] struct nl_msg *nlMessage, void *context) noexcept
{
    if (context == nullptr) {
        LOGE << "Received netlink route get address response with null context";
        return NL_SKIP;
    }

    auto &nlGetAddressResult = *static_cast<std::vector<std::string> *>(context);
    nlGetAddressResult = ParseNlGetAddressDumpResponse(nlMessage);

    LOGD << "Successfully parsed a netlink route get address response message";

    return NL_OK;
}
} // namespace detail

namespace Microsoft::Net::Netlink
{
NetlinkSocket
CreateNlRouteSocket()
{
    // Allocate a new netlink socket.
    auto socket = NetlinkSocket::Allocate();

    // Connect the socket to the route netlink family.
    const int ret = nl_connect(socket, NETLINK_ROUTE);
    if (ret < 0) {
        const auto errorCode = MakeNetlinkErrorCode(-ret);
        const auto message = std::format("Failed to connect netlink socket for nl control with error {}", errorCode.value());
        LOGE << message;
        throw std::system_error(errorCode, message);
    }

    return socket;
}

std::vector<std::string>
NetlinkEnumerateIpv4Addresses()
{
    // Allocate a new nlroute message for sending a dump r equest for all ipv4 addresses.
    auto nlMessageGetAddress{ NetlinkMessage::Allocate() };
    if (nlMessageGetAddress == nullptr) {
        LOGE << "Failed to allocate netlink message for get address request";
        return {};
    }

    // Populate the message with the get address request.
    struct ifaddrmsg *ifAddressMessage{ nullptr };
    struct nlmsghdr *nlMessageGetAddressHeader = nlmsg_hdr(nlMessageGetAddress);
    nlMessageGetAddressHeader->nlmsg_len = NLMSG_LENGTH(sizeof *ifAddressMessage);
    nlMessageGetAddressHeader->nlmsg_type = RTM_GETADDR;
    nlMessageGetAddressHeader->nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    nlMessageGetAddressHeader->nlmsg_seq = NL_AUTO_SEQ;
    nlMessageGetAddressHeader->nlmsg_pid = NL_AUTO_PID;

    // Filter the requested addresses to only include IPv4 addresses.
    ifAddressMessage = static_cast<struct ifaddrmsg *>(NLMSG_DATA(nlMessageGetAddressHeader));
    ifAddressMessage->ifa_family = AF_INET;

    auto nlRouteSocket{ CreateNlRouteSocket() };
    std::vector<std::string> ipAddresses{};

    // Configure the callback to invoke to process the response.
    int ret = nl_socket_modify_cb(nlRouteSocket, NL_CB_VALID, NL_CB_CUSTOM, detail::HandleNlGetAddressResponse, &ipAddresses);
    if (ret < 0) {
        LOGE << std::format("Failed to modify netlink socket callback with error {}", ret);
        return {};
    }

    // Send the request.
    ret = nl_send_auto(nlRouteSocket, nlMessageGetAddress);
    if (ret < 0) {
        LOGE << std::format("Failed to send netlink get address message with error {}", ret);
        return {};
    }

    // Receive the response, which will invoke the configured callback.
    ret = nl_recvmsgs_default(nlRouteSocket);
    if (ret < 0) {
        LOGE << std::format("Failed to receive netlink get address response with error {} ({})", ret, nl_geterror(ret));
        return {};
    }

    return ipAddresses;
}

std::optional<std::vector<std::string>>
NetlinkEnumerateIpv4Addresses2()
{
    auto nlRouteSocket{ CreateNlRouteSocket() };

    struct nl_cache *ipAddressCache{ nullptr };
    int ret = rtnl_addr_alloc_cache(nlRouteSocket, &ipAddressCache);
    if (ret != 0) {
        LOGE << std::format("failed to allocate address cache with error {} ({})", ret, nl_geterror(ret));
        return std::nullopt;
    }

    std::vector<std::string> ipAddresses{};
    struct nl_object *nlObjectIpAddress{ nullptr };
    for (nlObjectIpAddress = nl_cache_get_first(ipAddressCache); nlObjectIpAddress != nullptr;  nlObjectIpAddress = nl_cache_get_next(nlObjectIpAddress)) {
        auto *nlIpAddress = reinterpret_cast<struct rtnl_addr *>(nlObjectIpAddress);

        const auto family = rtnl_addr_get_family(nlIpAddress);
        switch (family) {
        case AF_INET: {
            auto *ipAddress = rtnl_addr_get_local(nlIpAddress);
            std::string ipAddressAscii(INET_ADDRSTRLEN, '\0');
            nl_addr2str(ipAddress, std::data(ipAddressAscii), std::size(ipAddressAscii));
            ipAddresses.push_back(ipAddressAscii);
            break;
        }
        default: {
            break;
        }
        }
    }

    return ipAddresses;
}
} // namespace Microsoft::Net::Netlink
