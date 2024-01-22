
#include <algorithm>
#include <array>
#include <cerrno>
#include <format>
#include <ranges>
#include <stdexcept>
#include <string_view>

#include <linux/if.h>
#include <magic_enum.hpp>
#include <microsoft/net/netlink/nl80211/Netlink80211.hxx>
#include <microsoft/net/netlink/nl80211/Netlink80211Interface.hxx>
#include <microsoft/net/wifi/AccessPointDiscoveryAgentOperationsNetlink.hxx>
#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <netlink/genl/genl.h>
#include <netlink/msg.h>
#include <netlink/netlink.h>
#include <netlink/socket.h>
#include <notstd/Scope.hxx>
#include <plog/Log.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/types.h>
#include <unistd.h>

using namespace Microsoft::Net::Wifi;
using namespace Microsoft::Net::Netlink::Nl80211;

using Microsoft::Net::Netlink::NetlinkMessage;
using Microsoft::Net::Netlink::NetlinkSocket;
using Microsoft::Net::Netlink::Nl80211::Nl80211ProtocolState;

AccessPointDiscoveryAgentOperationsNetlink::AccessPointDiscoveryAgentOperationsNetlink() :
    AccessPointDiscoveryAgentOperationsNetlink(nullptr) // FIXME: this c'tor should probably be removed
{}

AccessPointDiscoveryAgentOperationsNetlink::AccessPointDiscoveryAgentOperationsNetlink(std::shared_ptr<IAccessPointFactory> accessPointFactory) :
    m_accessPointFactory(std::move(accessPointFactory)),
    m_cookie(CookieValid),
    m_netlink80211ProtocolState(Nl80211ProtocolState::Instance())
{}

AccessPointDiscoveryAgentOperationsNetlink::~AccessPointDiscoveryAgentOperationsNetlink()
{
    RequestNetlinkProcessingLoopStop();

    // Explicitly stop the netlink message processing thread to force the thread
    // to tear down predictably. This isn't technically required since the
    // member variables are ordered such that the thread will be stopped before
    // the netlink socket is destroyed, but it is good practice to be explicit
    // and avoids any potential issues if members are re-ordered.
    Stop();

    // Invalidate the cookie as a rudimentary way to detect use-after-free
    // within the netlink thread.
    m_cookie = CookieInvalid;
}

void
AccessPointDiscoveryAgentOperationsNetlink::RequestNetlinkProcessingLoopStop()
{
    if (m_eventLoopStopFd == -1) {
        return;
    }

    constexpr uint64_t StopValue{ 1 };

    // Write any value to the eventfd to signal the netlink processing loop to
    // stop. The value itself is meaningless, but the write to the file
    // descriptor will interrupt the epoll_wait call and set the stop flag.
    ssize_t numWritten = write(m_eventLoopStopFd, &StopValue, sizeof StopValue);
    if (numWritten != sizeof StopValue) {
        const auto err = errno;
        LOG_ERROR << std::format("Failed to write to event loop stop fd with error {} ({})", err, strerror(err));
    }
}

void
AccessPointDiscoveryAgentOperationsNetlink::Start(AccessPointPresenceEventCallback accessPointPresenceEventCallback)
{
    if (m_netlinkMessageProcessingThread.joinable()) {
        LOG_WARNING << "Netlink message processing thread is already running";
        Stop();
    }

    // TODO: This function needs to signal errors either through its return type, or an exception.

    // Allocate a new netlink socket for use with nl80211.
    auto nl80211SocketOpt{ CreateNl80211Socket() };
    if (nl80211SocketOpt == nullptr) {
        LOG_ERROR << "Failed to allocate new netlink socket for nl control";
        return;
    }

    auto nl80211Socket{ std::move(nl80211SocketOpt.value()) };

    // Subscribe to configuration messages.
    int nl80211MulticastGroupIdConfig = m_netlink80211ProtocolState.MulticastGroupId[Nl80211MulticastGroup::Configuration];
    int ret = nl_socket_add_membership(nl80211Socket, nl80211MulticastGroupIdConfig);
    if (ret < 0) {
        const auto err = errno;
        LOG_ERROR << std::format("Failed to add netlink socket membership for '" NL80211_MULTICAST_GROUP_CONFIG "' group with error {} ({})", err, strerror(err));
        return;
    }

    // Update the access point presence callback for the netlink message handler to use.
    // Note: This is not thread-safe.
    m_accessPointPresenceCallback = std::move(accessPointPresenceEventCallback);
    m_netlinkMessageProcessingThread = std::jthread([this, netlinkSocket = std::move(nl80211Socket)](std::stop_token stopToken) mutable {
        ProcessNetlinkMessagesThread(std::move(netlinkSocket), std::move(stopToken));
    });
}

void
AccessPointDiscoveryAgentOperationsNetlink::Start2(AccessPointPresenceEventCallback2 accessPointPresenceEventCallback)
{
    if (m_netlinkMessageProcessingThread.joinable()) {
        LOG_WARNING << "Netlink message processing thread is already running";
        Stop();
    }

    // TODO: This function needs to signal errors either through its return type, or an exception.

    // Allocate a new netlink socket for use with nl80211.
    auto nl80211SocketOpt{ CreateNl80211Socket() };
    if (nl80211SocketOpt == nullptr) {
        LOG_ERROR << "Failed to allocate new netlink socket for nl control";
        return;
    }

    auto nl80211Socket{ std::move(nl80211SocketOpt.value()) };

    // Subscribe to configuration messages.
    int nl80211MulticastGroupIdConfig = m_netlink80211ProtocolState.MulticastGroupId[Nl80211MulticastGroup::Configuration];
    int ret = nl_socket_add_membership(nl80211Socket, nl80211MulticastGroupIdConfig);
    if (ret < 0) {
        const auto err = errno;
        LOG_ERROR << std::format("Failed to add netlink socket membership for '" NL80211_MULTICAST_GROUP_CONFIG "' group with error {} ({})", err, strerror(err));
        return;
    }

    // Update the access point presence callback for the netlink message handler to use.
    // Note: This is not thread-safe.
    m_accessPointPresenceCallback2 = std::move(accessPointPresenceEventCallback);
    m_netlinkMessageProcessingThread = std::jthread([this, netlinkSocket = std::move(nl80211Socket)](std::stop_token stopToken) mutable {
        ProcessNetlinkMessagesThread(std::move(netlinkSocket), std::move(stopToken));
    });
}

void
AccessPointDiscoveryAgentOperationsNetlink::Stop()
{
    if (m_netlinkMessageProcessingThread.joinable()) {
        LOG_VERBOSE << "Stopping netlink message processing thread";
        RequestNetlinkProcessingLoopStop();
        m_netlinkMessageProcessingThread.request_stop();
        m_netlinkMessageProcessingThread.join();
    }
}

namespace detail
{
/**
 * @brief Helper function to determine if an nl80211 interface is an AP. To be used in range expressions.
 *
 * @param nl80211Interface
 * @return true
 * @return false
 */
bool
IsNl80211InterfaceTypeAp(const Nl80211Interface &nl80211Interface)
{
    return (nl80211Interface.Type == nl80211_iftype::NL80211_IFTYPE_AP);
}

/**
 * @brief Helper function returning the name of an nl80211 interface. To be used in range expressions.
 *
 * @param nl80211Interface
 * @return std::string
 */
std::string
Nl80211InterfaceName(const Nl80211Interface &nl80211Interface)
{
    return nl80211Interface.Name;
}
} // namespace detail

std::future<std::vector<std::string>>
AccessPointDiscoveryAgentOperationsNetlink::ProbeAsync()
{
    std::promise<std::vector<std::string>> probePromise{};
    auto probeFuture = probePromise.get_future();

    // Enumerate all nl80211 interfaces and filter out those that are not APs.
    auto nl80211Interfaces{ Nl80211Interface::Enumerate() };
    auto nl80211ApInterfaceNames = nl80211Interfaces | std::views::filter(detail::IsNl80211InterfaceTypeAp) | std::views::transform(detail::Nl80211InterfaceName);
    std::vector<std::string> accessPoints(std::make_move_iterator(std::begin(nl80211ApInterfaceNames)), std::make_move_iterator(std::end(nl80211ApInterfaceNames)));

    // Clear the vector since most of the items were moved out.
    nl80211Interfaces.clear();

    probePromise.set_value(std::move(accessPoints));

    return probeFuture;
}

std::future<std::vector<std::shared_ptr<IAccessPoint>>>
AccessPointDiscoveryAgentOperationsNetlink::ProbeAsync2()
{
    std::promise<std::vector<std::shared_ptr<IAccessPoint>>> probePromise{};
    auto probeFuture = probePromise.get_future();

    return probeFuture;
}

int
AccessPointDiscoveryAgentOperationsNetlink::ProcessNetlinkMessage(struct nl_msg *netlinkMessage, AccessPointPresenceEventCallback &accessPointPresenceEventCallback)
{
    // Ensure the message has a genl header.
    auto *netlinkMessageHeader{ static_cast<struct nlmsghdr *>(nlmsg_hdr(netlinkMessage)) };
    if (genlmsg_valid_hdr(netlinkMessageHeader, 1) == 0) {
        LOG_ERROR << "Netlink genl message header is invalid, ignoring message";
        return NL_SKIP;
    }

    // Extract the nl80211 (genl) message header.
    const auto *genlMessageHeader{ static_cast<struct genlmsghdr *>(nlmsg_data(netlinkMessageHeader)) };
    const auto nl80211CommandName{ Nl80211CommandToString(static_cast<nl80211_commands>(genlMessageHeader->cmd)) };

    // Parse the message attributes.
    std::array<struct nlattr *, NL80211_ATTR_MAX + 1> netlinkMessageAttributes{};
    int ret = nla_parse(std::data(netlinkMessageAttributes), std::size(netlinkMessageAttributes), genlmsg_attrdata(genlMessageHeader, 0), genlmsg_attrlen(genlMessageHeader, 0), nullptr);
    if (ret < 0) {
        LOG_ERROR << std::format("Failed to parse netlink message attributes with error {} ({})", ret, strerror(-ret));
        return NL_SKIP;
    }

    int interfaceIndex{ -1 };
    const char *interfaceName{ nullptr };
    nl80211_iftype interfaceType{ NL80211_IFTYPE_UNSPECIFIED };
    AccessPointPresenceEvent accessPointPresenceEvent;

    LOG_VERBOSE << std::format("Received {} nl80211 command message", nl80211CommandName);

    switch (genlMessageHeader->cmd) {
    case NL80211_CMD_NEW_INTERFACE:
    case NL80211_CMD_DEL_INTERFACE: {
        interfaceIndex = static_cast<int32_t>(nla_get_u32(netlinkMessageAttributes[NL80211_ATTR_IFINDEX]));
        interfaceName = static_cast<const char *>(nla_data(netlinkMessageAttributes[NL80211_ATTR_IFNAME]));
        interfaceType = static_cast<nl80211_iftype>(nla_get_u32(netlinkMessageAttributes[NL80211_ATTR_IFTYPE]));
        if (interfaceType != NL80211_IFTYPE_AP) {
            LOG_VERBOSE << std::format("Ignoring interface presence change nl80211 message for non-ap wi-fi interface (type={})", Nl80211InterfaceTypeToString(interfaceType));
            return NL_SKIP;
        }
        accessPointPresenceEvent = (genlMessageHeader->cmd == NL80211_CMD_NEW_INTERFACE) ? AccessPointPresenceEvent::Arrived : AccessPointPresenceEvent::Departed;
        break;
    }
    case NL80211_CMD_SET_INTERFACE: {
        interfaceIndex = static_cast<int32_t>(nla_get_u32(netlinkMessageAttributes[NL80211_ATTR_IFINDEX]));
        interfaceName = static_cast<const char *>(nla_data(netlinkMessageAttributes[NL80211_ATTR_IFNAME]));
        interfaceType = static_cast<nl80211_iftype>(nla_get_u32(netlinkMessageAttributes[NL80211_ATTR_IFTYPE]));
        accessPointPresenceEvent = (interfaceType == NL80211_IFTYPE_AP) ? AccessPointPresenceEvent::Arrived : AccessPointPresenceEvent::Departed;
        break;
    }
    default: {
        PLOG_VERBOSE << std::format("Ignoring {} nl80211 command message", nl80211CommandName);
        return NL_SKIP;
    }
    }

    // Update map tracking interface information. Handle the case where the
    // interface is already present. This happens for NL80211_CMD_SET_INTERFACE
    // when the interface type did not change.
    auto interfaceInfo = m_interfaceInfo.find(interfaceIndex);
    if (interfaceInfo == std::cend(m_interfaceInfo)) {
        if (accessPointPresenceEvent == AccessPointPresenceEvent::Arrived) {
            m_interfaceInfo[interfaceIndex] = { interfaceName, interfaceType };
        }
    } else {
        if (accessPointPresenceEvent == AccessPointPresenceEvent::Departed) {
            m_interfaceInfo.erase(interfaceInfo);
        }
    }

    // Invoke presence event callback if present.
    if (accessPointPresenceEventCallback != nullptr) {
        LOGV << std::format("Invoking access point presence event callback with event args 'interface={}, presence={}'", interfaceName, magic_enum::enum_name(accessPointPresenceEvent));
        accessPointPresenceEventCallback(accessPointPresenceEvent, interfaceName);
    }

    return NL_OK;
}

int
AccessPointDiscoveryAgentOperationsNetlink::ProcessNetlinkMessage2(struct nl_msg *netlinkMessage, AccessPointPresenceEventCallback2 &accessPointPresenceEventCallback)
{
    // Ensure the message has a genl header.
    auto *netlinkMessageHeader{ static_cast<struct nlmsghdr *>(nlmsg_hdr(netlinkMessage)) };
    if (genlmsg_valid_hdr(netlinkMessageHeader, 1) == 0) {
        LOG_ERROR << "Netlink genl message header is invalid, ignoring message";
        return NL_SKIP;
    }

    // Extract the nl80211 (genl) message header.
    const auto *genlMessageHeader{ static_cast<struct genlmsghdr *>(nlmsg_data(netlinkMessageHeader)) };
    const auto nl80211CommandName{ Nl80211CommandToString(static_cast<nl80211_commands>(genlMessageHeader->cmd)) };

    // Parse the message attributes.
    std::array<struct nlattr *, NL80211_ATTR_MAX + 1> netlinkMessageAttributes{};
    int ret = nla_parse(std::data(netlinkMessageAttributes), std::size(netlinkMessageAttributes), genlmsg_attrdata(genlMessageHeader, 0), genlmsg_attrlen(genlMessageHeader, 0), nullptr);
    if (ret < 0) {
        LOG_ERROR << std::format("Failed to parse netlink message attributes with error {} ({})", ret, strerror(-ret));
        return NL_SKIP;
    }

    int interfaceIndex{ -1 };
    const char *interfaceName{ nullptr };
    nl80211_iftype interfaceType{ NL80211_IFTYPE_UNSPECIFIED };
    AccessPointPresenceEvent accessPointPresenceEvent;

    LOG_VERBOSE << std::format("Received {} nl80211 command message", nl80211CommandName);

    switch (genlMessageHeader->cmd) {
    case NL80211_CMD_NEW_INTERFACE:
    case NL80211_CMD_DEL_INTERFACE: {
        interfaceIndex = static_cast<int32_t>(nla_get_u32(netlinkMessageAttributes[NL80211_ATTR_IFINDEX]));
        interfaceName = static_cast<const char *>(nla_data(netlinkMessageAttributes[NL80211_ATTR_IFNAME]));
        interfaceType = static_cast<nl80211_iftype>(nla_get_u32(netlinkMessageAttributes[NL80211_ATTR_IFTYPE]));
        if (interfaceType != NL80211_IFTYPE_AP) {
            LOG_VERBOSE << std::format("Ignoring interface presence change nl80211 message for non-ap wi-fi interface (type={})", Nl80211InterfaceTypeToString(interfaceType));
            return NL_SKIP;
        }
        accessPointPresenceEvent = (genlMessageHeader->cmd == NL80211_CMD_NEW_INTERFACE) ? AccessPointPresenceEvent::Arrived : AccessPointPresenceEvent::Departed;
        break;
    }
    case NL80211_CMD_SET_INTERFACE: {
        interfaceIndex = static_cast<int32_t>(nla_get_u32(netlinkMessageAttributes[NL80211_ATTR_IFINDEX]));
        interfaceName = static_cast<const char *>(nla_data(netlinkMessageAttributes[NL80211_ATTR_IFNAME]));
        interfaceType = static_cast<nl80211_iftype>(nla_get_u32(netlinkMessageAttributes[NL80211_ATTR_IFTYPE]));
        accessPointPresenceEvent = (interfaceType == NL80211_IFTYPE_AP) ? AccessPointPresenceEvent::Arrived : AccessPointPresenceEvent::Departed;
        break;
    }
    default: {
        PLOG_VERBOSE << std::format("Ignoring {} nl80211 command message", nl80211CommandName);
        return NL_SKIP;
    }
    }

    // Update map tracking interface information. Handle the case where the
    // interface is already present. This happens for NL80211_CMD_SET_INTERFACE
    // when the interface type did not change.
    auto interfaceInfo = m_interfaceInfo.find(interfaceIndex);
    if (interfaceInfo == std::cend(m_interfaceInfo)) {
        if (accessPointPresenceEvent == AccessPointPresenceEvent::Arrived) {
            m_interfaceInfo[interfaceIndex] = { interfaceName, interfaceType };
        }
    } else {
        if (accessPointPresenceEvent == AccessPointPresenceEvent::Departed) {
            m_interfaceInfo.erase(interfaceInfo);
        }
    }

    // Invoke presence event callback if present.
    if (accessPointPresenceEventCallback != nullptr) {
        LOGV << std::format("Invoking access point presence event callback with event args 'interface={}, presence={}'", interfaceName, magic_enum::enum_name(accessPointPresenceEvent));
        auto accessPoint = m_accessPointFactory->Create(interfaceName);
        accessPointPresenceEventCallback(accessPointPresenceEvent, accessPoint);
    }

    return NL_OK;
}

/* static */
int
AccessPointDiscoveryAgentOperationsNetlink::ProcessNetlinkMessagesCallback(struct nl_msg *netlinkMessage, void *contextArgument)
{
    LOG_VERBOSE << "Received netlink message callback";

    // Validate the context argument and cookie to ensure the discovery agent is still alive.
    auto *instance{ static_cast<AccessPointDiscoveryAgentOperationsNetlink *>(contextArgument) };
    if (instance == nullptr) {
        static constexpr auto errorMessage{ "Netlink message callback context is null; this is a bug!" };
        LOG_FATAL << errorMessage;
        throw std::runtime_error(errorMessage);
    } else if (instance->m_cookie != CookieValid) {
        LOG_ERROR << "Netlink message callback context cookie is invalid; discovery agent instance has been destroyed";
        return NL_STOP;
    }

    auto ret = instance->ProcessNetlinkMessage(netlinkMessage, instance->m_accessPointPresenceCallback);
    ret = instance->ProcessNetlinkMessage2(netlinkMessage, instance->m_accessPointPresenceCallback2);
    LOG_VERBOSE << std::format("Processed netlink message with result {}", ret);

    return ret;
}

void
AccessPointDiscoveryAgentOperationsNetlink::ProcessNetlinkMessagesThread(NetlinkSocket netlinkSocket, std::stop_token stopToken)
{
    // Disable sequence number checking since it is not required for event notifications.
    nl_socket_disable_seq_check(netlinkSocket);

    // Register a callback to be invoked for all valid, parsed netlink messages.
    int ret = nl_socket_modify_cb(netlinkSocket, NL_CB_VALID, NL_CB_CUSTOM, ProcessNetlinkMessagesCallback, this);
    if (ret < 0) {
        const auto err = errno;
        LOG_ERROR << std::format("Failed to modify netlink socket callback with error {} ({})", err, strerror(err));
        return;
    }

    // Put the socket into non-blocking mode since callbacks have been configured.
    ret = nl_socket_set_nonblocking(netlinkSocket);
    if (ret < 0) {
        const auto err = errno;
        LOG_ERROR << std::format("Failed to set netlink socket to non-blocking mode with error {} ({})", err, strerror(err));
        return;
    }

    // Obtain the underlying file descriptor for the netlink socket.
    auto netlinkSocketFileDescriptor = nl_socket_get_fd(netlinkSocket);
    if (netlinkSocketFileDescriptor < 0) {
        const auto err = errno;
        LOG_ERROR << std::format("Failed to obtain netlink socket file descriptor with error {} ({})", err, strerror(err));
        return;
    }

    // Configure epoll to monitor the netlink socket for readability.
    int fdEpoll = epoll_create1(0);
    if (fdEpoll < 0) {
        const auto err = errno;
        LOG_ERROR << std::format("Failed to create epoll instance with error {} ({})", err, strerror(err));
        return;
    }

    // Close the epoll file descriptor when this function returns.
    auto closeEpollOnExit = notstd::ScopeExit([fdEpoll] {
        close(fdEpoll);
    });

    // Allocate space for two events: one for the netlink socket and one for the eventfd to stop the event loop.
    static constexpr int EpollEventsMax{ 2 };
    struct epoll_event events[EpollEventsMax] = {};

    // Populate the epoll event for the netlink socket.
    struct epoll_event *epollEventNetlinkSocket = &events[0];
    epollEventNetlinkSocket->events = EPOLLIN;
    epollEventNetlinkSocket->data.fd = netlinkSocketFileDescriptor;

    // Register the netlink socket event with epoll.
    ret = epoll_ctl(fdEpoll, EPOLL_CTL_ADD, netlinkSocketFileDescriptor, epollEventNetlinkSocket);
    if (ret < 0) {
        const auto err = errno;
        LOG_ERROR << std::format("Failed to register netlink socket event with epoll with error {} ({})", err, strerror(err));
        return;
    }

    // Create a new epoll event to allow stopping the processing loop using eventfd.
    int eventLoopStopFd = eventfd(0, 0);
    if (eventLoopStopFd < 0) {
        const auto err = errno;
        LOG_ERROR << std::format("Failed to create eventfd for stopping event loop with error {} ({})", err, strerror(err));
        return;
    }

    // Close the eventfd when this function returns and clear out the member.
    auto closeNetlinkMessageProcessingLoopStopFdOnExit = notstd::ScopeExit([this, eventLoopStopFd] {
        close(eventLoopStopFd);
        m_eventLoopStopFd = -1;
    });

    // Populate the epoll event for the event loop stop file descriptor.
    struct epoll_event *epollEventLoopStopFd = &events[1];
    epollEventLoopStopFd->events = EPOLLIN;
    epollEventLoopStopFd->data.fd = eventLoopStopFd;

    // Register the event loop stop event with epoll.
    ret = epoll_ctl(fdEpoll, EPOLL_CTL_ADD, eventLoopStopFd, epollEventLoopStopFd);
    if (ret < 0) {
        const auto err = errno;
        LOG_ERROR << std::format("Failed to register event loop stop fd event with epoll with error {} ({})", err, strerror(err));
        return;
    }

    // Save the event loop stop fd so it can be signaled externally to stop the event loop.
    m_eventLoopStopFd = eventLoopStopFd;

    bool stopRequested = false;

    // Event loop to wait for netlink messages and process them.
    for (;;) {
        if (stopToken.stop_requested() || stopRequested) {
            LOG_VERBOSE << "Netlink message processing thread has been requested to stop";
            break;
        }

        // Wait for at least one event file descriptor to become ready.
        int numEventsReady = epoll_wait(fdEpoll, events, EpollEventsMax, -1);
        if (numEventsReady < 0) {
            const auto err = errno;
            if (err == EINTR) {
                LOG_VERBOSE << "Interrupted while waiting for epoll events, retrying";
                continue;
            }

            LOG_ERROR << std::format("Failed to wait for epoll events with error {} ({})", err, strerror(err));
            break;
        }

        // Determine which file descriptor(s) became ready and handle them.
        for (auto i = 0; i < numEventsReady; i++) {
            const auto &eventReady = events[i];
            if (eventReady.data.fd == netlinkSocketFileDescriptor) {
                HandleNetlinkSocketReady(netlinkSocket);
            } else if (eventReady.data.fd == eventLoopStopFd) {
                stopRequested = true;
            } else {
                LOG_WARNING << std::format("Unknown file descriptor {} is ready for reading", eventReady.data.fd);
            }
        }
    }

    LOG_INFO << "Netlink message processing thread has exited";
}

void
AccessPointDiscoveryAgentOperationsNetlink::HandleNetlinkSocketReady(NetlinkSocket &netlinkSocket)
{
    LOG_VERBOSE << "Handling netlink socket read availabilty";

    // Read all pending netlink messages.
    for (;;) {
        int ret = nl_recvmsgs_default(netlinkSocket);
        if (ret < 0) {
            const auto err = errno;
            if (err == EINTR) {
                LOG_VERBOSE << "Interrupted while waiting for netlink messages, retrying";
                continue;
            } else {
                LOG_ERROR << std::format("Failed to receive netlink messages with error {} ({})", err, strerror(err));
                break;
            }
        } else {
            LOG_VERBOSE << "Successfully processed netlink messages";
            break;
        }
    }
}
