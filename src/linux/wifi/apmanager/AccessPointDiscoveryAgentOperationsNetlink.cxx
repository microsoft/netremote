#include <array>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <format>
#include <future>
#include <iterator>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <stop_token>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#include <linux/genetlink.h>
#include <linux/netlink.h>
#include <linux/nl80211.h>
#include <magic_enum.hpp>
#include <microsoft/net/netlink/NetlinkException.hxx>
#include <microsoft/net/netlink/NetlinkSocket.hxx>
#include <microsoft/net/netlink/nl80211/Netlink80211.hxx>
#include <microsoft/net/netlink/nl80211/Netlink80211Interface.hxx>
#include <microsoft/net/netlink/nl80211/Netlink80211ProtocolState.hxx>
#include <microsoft/net/wifi/AccessPointDiscoveryAgentOperationsNetlink.hxx>
#include <microsoft/net/wifi/AccessPointLinux.hxx>
#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPointDiscoveryAgentOperations.hxx>
#include <netlink/attr.h>
#include <netlink/genl/genl.h>
#include <netlink/handlers.h>
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
using namespace Microsoft::Net::Netlink;
using namespace Microsoft::Net::Netlink::Nl80211;

using Microsoft::Net::Netlink::NetlinkSocket;
using Microsoft::Net::Netlink::Nl80211::Nl80211ProtocolState;

// NOLINTBEGIN(concurrency-mt-unsafe)

AccessPointDiscoveryAgentOperationsNetlink::AccessPointDiscoveryAgentOperationsNetlink(std::shared_ptr<AccessPointFactoryLinux> accessPointFactory) :
    m_accessPointFactory(std::move(accessPointFactory)),
    m_cookie(CookieValid),
    m_netlink80211ProtocolState(Nl80211ProtocolState::Instance())
{}

AccessPointDiscoveryAgentOperationsNetlink::~AccessPointDiscoveryAgentOperationsNetlink()
{
    RequestNetlinkProcessingLoopStop();

    // Explicitly stop the netlink message processing thread to force the thread to tear down predictably. This isn't
    // technically required since the member variables are ordered such that the thread will be stopped before the
    // netlink socket is destroyed, but it is good practice to be explicit and avoids any potential issues if members
    // are re-ordered. Since Stop() is a virtual function, it is invoked on the derived (this) class directly here to
    // avoid any issues with the vtable.
    AccessPointDiscoveryAgentOperationsNetlink::Stop();

    // Invalidate the cookie as a rudimentary way to detect use-after-free
    // within the netlink thread.
    m_cookie = CookieInvalid;
}

void
AccessPointDiscoveryAgentOperationsNetlink::RequestNetlinkProcessingLoopStop() const
{
    if (m_eventLoopStopFd == -1) {
        return;
    }

    constexpr uint64_t StopValue{ 1 };

    // Write any value to the eventfd to signal the netlink processing loop to
    // stop. The value itself is meaningless, but the write to the file
    // descriptor will interrupt the epoll_wait call and set the stop flag.
    const ssize_t numWritten = write(m_eventLoopStopFd, &StopValue, sizeof StopValue);
    if (numWritten != sizeof StopValue) {
        const auto err = errno;
        LOGE << std::format("Failed to write to event loop stop fd with error {} ({})", err, strerror(err));
    }
}

void
AccessPointDiscoveryAgentOperationsNetlink::Start(AccessPointPresenceEventCallback accessPointPresenceEventCallback)
{
    if (m_netlinkMessageProcessingThread.joinable()) {
        LOGW << "Netlink message processing thread is already running";
        Stop();
    }

    // Subscribe to configuration messages.
    auto nl80211Socket{ CreateNl80211Socket() };
    const int nl80211MulticastGroupIdConfig = m_netlink80211ProtocolState.MulticastGroupId[Nl80211MulticastGroup::Configuration];
    const int ret = nl_socket_add_membership(nl80211Socket, nl80211MulticastGroupIdConfig);
    if (ret < 0) {
        throw NetlinkException::CreateLogged(-ret, "Failed to add netlink socket membership for 'NL80211_MULTICAST_GROUP_CONFIG'");
    }

    // Update the access point presence callback for the netlink message handler to use.
    // Note: This is not thread-safe.
    m_accessPointPresenceCallback = std::move(accessPointPresenceEventCallback);
    m_netlinkMessageProcessingThread = std::jthread([this, netlinkSocket = std::move(nl80211Socket)](std::stop_token stopToken) mutable {
        ProcessNetlinkMessagesThread(std::move(netlinkSocket), std::move(stopToken));
    });
}

void
AccessPointDiscoveryAgentOperationsNetlink::Stop()
{
    if (m_netlinkMessageProcessingThread.joinable()) {
        LOGD << "Stopping netlink message processing thread";
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
 * @param nl80211Interface The nl80211 interface to check.
 * @return true
 * @return false
 */
bool
IsNl80211InterfaceTypeAp(const Nl80211Interface &nl80211Interface)
{
    return (nl80211Interface.Type == nl80211_iftype::NL80211_IFTYPE_AP);
}

/**
 * @brief Helper function to create an access point instance from an nl80211 interface. To be used in range expressions.
 *
 * @param accessPointFactory The factory to use for creating the access point instance.
 * @param nl80211Interface The nl80211 interface to create the access point instance from.
 * @return std::shared_ptr<IAccessPoint>
 */
std::shared_ptr<IAccessPoint>
MakeAccessPoint(const std::shared_ptr<AccessPointFactoryLinux> &accessPointFactory, const Nl80211Interface &nl80211Interface)
{
    auto createArgs = std::make_unique<AccessPointCreateArgsLinux>(nl80211Interface);
    return accessPointFactory->Create(nl80211Interface.Name, std::move(createArgs));
}
} // namespace detail

std::future<std::vector<std::shared_ptr<IAccessPoint>>>
AccessPointDiscoveryAgentOperationsNetlink::ProbeAsync()
{
    const auto MakeAccessPoint = [this](const Nl80211Interface &nl80211Interface) {
        return detail::MakeAccessPoint(m_accessPointFactory, nl80211Interface);
    };

    std::promise<std::vector<std::shared_ptr<IAccessPoint>>> probePromise{};
    auto probeFuture = probePromise.get_future();

    // Enumerate all nl80211 interfaces and filter out those that are not APs.
    auto nl80211Interfaces{ Nl80211Interface::Enumerate() };
    auto accessPointsView = nl80211Interfaces | std::views::filter(detail::IsNl80211InterfaceTypeAp) | std::views::transform(MakeAccessPoint);
    std::vector<std::shared_ptr<IAccessPoint>> accessPoints(std::make_move_iterator(std::begin(accessPointsView)), std::make_move_iterator(std::end(accessPointsView)));

    // Clear the vector since most of the items were moved out.
    nl80211Interfaces.clear();

    probePromise.set_value(std::move(accessPoints));

    return probeFuture;
}

int
AccessPointDiscoveryAgentOperationsNetlink::ProcessNetlinkMessage(struct nl_msg *netlinkMessage, AccessPointPresenceEventCallback &accessPointPresenceEventCallback)
{
    // Ensure the message has a genl header.
    auto *netlinkMessageHeader{ static_cast<struct nlmsghdr *>(nlmsg_hdr(netlinkMessage)) };
    if (genlmsg_valid_hdr(netlinkMessageHeader, 1) == 0) {
        LOGE << "Netlink genl message header is invalid, ignoring message";
        return NL_SKIP;
    }

    // Extract the nl80211 (genl) message header.
    const auto *genlMessageHeader{ static_cast<struct genlmsghdr *>(nlmsg_data(netlinkMessageHeader)) };
    const auto nl80211CommandName{ Nl80211CommandToString(static_cast<nl80211_commands>(genlMessageHeader->cmd)) };

    // Parse the message attributes.
    std::array<struct nlattr *, NL80211_ATTR_MAX + 1> netlinkMessageAttributes{};
    int ret = nla_parse(std::data(netlinkMessageAttributes), std::size(netlinkMessageAttributes) - 1, genlmsg_attrdata(genlMessageHeader, 0), genlmsg_attrlen(genlMessageHeader, 0), nullptr);
    if (ret < 0) {
        LOGE << std::format("Failed to parse netlink message attributes with error {} ({})", ret, strerror(-ret));
        return NL_SKIP;
    }

    if (genlMessageHeader->cmd != NL80211_CMD_NEW_INTERFACE &&
        genlMessageHeader->cmd != NL80211_CMD_DEL_INTERFACE &&
        genlMessageHeader->cmd != NL80211_CMD_SET_INTERFACE) {
        LOGD << std::format("Ignoring {} nl80211 command message", nl80211CommandName);
        return NL_SKIP;
    }

    LOGD << std::format("Received {} nl80211 command message", nl80211CommandName);

    // Parse the message into an Nl80211Interface object.
    auto nl80211InterfaceOpt = Nl80211Interface::Parse(netlinkMessage);
    if (!nl80211InterfaceOpt.has_value()) {
        LOGE << "Failed to parse nl80211 interface dump response";
        return NL_OK;
    }

    AccessPointPresenceEvent accessPointPresenceEvent{};
    auto &nl80211Interface = nl80211InterfaceOpt.value();

    switch (genlMessageHeader->cmd) {
    case NL80211_CMD_NEW_INTERFACE:
    case NL80211_CMD_DEL_INTERFACE: {
        if (!nl80211Interface.IsAccessPoint()) {
            LOGD << std::format("Ignoring interface presence change nl80211 message for non-ap wi-fi interface (type={})", Nl80211InterfaceTypeToString(nl80211Interface.Type));
            return NL_OK;
        }
        accessPointPresenceEvent = (genlMessageHeader->cmd == NL80211_CMD_NEW_INTERFACE) ? AccessPointPresenceEvent::Arrived : AccessPointPresenceEvent::Departed;
        break;
    }
    case NL80211_CMD_SET_INTERFACE: {
        accessPointPresenceEvent = nl80211Interface.IsAccessPoint() ? AccessPointPresenceEvent::Arrived : AccessPointPresenceEvent::Departed;
        break;
    }
    default: {
        LOGD << std::format("Ignoring {} nl80211 command message", nl80211CommandName);
        return NL_SKIP;
    }
    }

    // Update interface seen cache, handling the case where the interface has already been seen. This happens for
    // NL80211_CMD_SET_INTERFACE whern the interface type did not change.
    auto interfaceSeenIterator = m_interfacesSeen.find(nl80211Interface);
    if (interfaceSeenIterator == std::cend(m_interfacesSeen)) {
        if (accessPointPresenceEvent == AccessPointPresenceEvent::Arrived) {
            m_interfacesSeen.insert(nl80211Interface);
        }
    } else {
        if (accessPointPresenceEvent == AccessPointPresenceEvent::Departed) {
            m_interfacesSeen.erase(interfaceSeenIterator);
        }
    }

    // Invoke presence event callback if present.
    if (accessPointPresenceEventCallback != nullptr) {
        const auto interfaceName{ nl80211Interface.Name };
        LOGD << std::format("Invoking access point presence event callback with event args 'interface={}, presence={}'", interfaceName, magic_enum::enum_name(accessPointPresenceEvent));
        auto accessPointCreateArgs = std::make_unique<AccessPointCreateArgsLinux>(std::move(nl80211Interface));
        auto accessPoint = m_accessPointFactory->Create(interfaceName, std::move(accessPointCreateArgs));

        accessPointPresenceEventCallback(accessPointPresenceEvent, accessPoint);
    }

    return NL_OK;
}

/* static */
int
AccessPointDiscoveryAgentOperationsNetlink::ProcessNetlinkMessagesCallback(struct nl_msg *netlinkMessage, void *contextArgument)
{
    LOGD << "Received netlink message callback";

    // Validate the context argument and cookie to ensure the discovery agent is still alive.
    auto *instance{ static_cast<AccessPointDiscoveryAgentOperationsNetlink *>(contextArgument) };
    if (instance == nullptr) {
        static constexpr auto errorMessage{ "Netlink message callback context is null; this is a bug!" };
        LOGF << errorMessage;
        throw std::runtime_error(errorMessage);
    }
    if (instance->m_cookie != CookieValid) {
        LOGE << "Netlink message callback context cookie is invalid; discovery agent instance has been destroyed";
        return NL_STOP;
    }

    auto ret = instance->ProcessNetlinkMessage(netlinkMessage, instance->m_accessPointPresenceCallback);
    LOGD << std::format("Processed netlink message with result {}", ret);

    return ret;
}

// NOLINTBEGIN(performance-unnecessary-value-param)
// This function is not performance-critical and the std::stop_token passed as a value is required for the jthread constructor.
void
AccessPointDiscoveryAgentOperationsNetlink::ProcessNetlinkMessagesThread(NetlinkSocket netlinkSocket, std::stop_token stopToken)
// NOLINTEND(performance-unnecessary-value-param)
{
    // Disable sequence number checking since it is not required for event notifications.
    nl_socket_disable_seq_check(netlinkSocket);

    // Register a callback to be invoked for all valid, parsed netlink messages.
    int ret = nl_socket_modify_cb(netlinkSocket, NL_CB_VALID, NL_CB_CUSTOM, ProcessNetlinkMessagesCallback, this);
    if (ret < 0) {
        const auto err = errno;
        LOGE << std::format("Failed to modify netlink socket callback with error {} ({})", err, strerror(err));
        return;
    }

    // Put the socket into non-blocking mode since callbacks have been configured.
    ret = nl_socket_set_nonblocking(netlinkSocket);
    if (ret < 0) {
        const auto err = errno;
        LOGE << std::format("Failed to set netlink socket to non-blocking mode with error {} ({})", err, strerror(err));
        return;
    }

    // Obtain the underlying file descriptor for the netlink socket.
    auto netlinkSocketFileDescriptor = nl_socket_get_fd(netlinkSocket);
    if (netlinkSocketFileDescriptor < 0) {
        const auto err = errno;
        LOGE << std::format("Failed to obtain netlink socket file descriptor with error {} ({})", err, strerror(err));
        return;
    }

    // Configure epoll to monitor the netlink socket for readability.
    const int fdEpoll = epoll_create1(0);
    if (fdEpoll < 0) {
        const auto err = errno;
        LOGE << std::format("Failed to create epoll instance with error {} ({})", err, strerror(err));
        return;
    }

    // Close the epoll file descriptor when this function returns.
    auto closeEpollOnExit = notstd::ScopeExit([fdEpoll] {
        close(fdEpoll);
    });

    // Allocate space for two events: one for the netlink socket and one for the eventfd to stop the event loop.
    static constexpr int EpollEventsMax{ 2 };
    std::array<epoll_event, EpollEventsMax> events{};

    // Populate the epoll event for the netlink socket.
    struct epoll_event *epollEventNetlinkSocket = std::data(events);
    epollEventNetlinkSocket->events = EPOLLIN;
    epollEventNetlinkSocket->data.fd = netlinkSocketFileDescriptor;

    // Register the netlink socket event with epoll.
    ret = epoll_ctl(fdEpoll, EPOLL_CTL_ADD, netlinkSocketFileDescriptor, epollEventNetlinkSocket);
    if (ret < 0) {
        const auto err = errno;
        LOGE << std::format("Failed to register netlink socket event with epoll with error {} ({})", err, strerror(err));
        return;
    }

    // Create a new epoll event to allow stopping the processing loop using eventfd.
    const int eventLoopStopFd = eventfd(0, 0);
    if (eventLoopStopFd < 0) {
        const auto err = errno;
        LOGE << std::format("Failed to create eventfd for stopping event loop with error {} ({})", err, strerror(err));
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
        LOGE << std::format("Failed to register event loop stop fd event with epoll with error {} ({})", err, strerror(err));
        return;
    }

    // Save the event loop stop fd so it can be signaled externally to stop the event loop.
    m_eventLoopStopFd = eventLoopStopFd;

    bool stopRequested = false;

    // Event loop to wait for netlink messages and process them.
    for (;;) {
        if (stopToken.stop_requested() || stopRequested) {
            LOGD << "Netlink message processing thread has been requested to stop";
            break;
        }

        // Wait for at least one event file descriptor to become ready.
        const int numEventsReady = epoll_wait(fdEpoll, std::data(events), EpollEventsMax, -1);
        if (numEventsReady < 0) {
            const auto err = errno;
            if (err == EINTR) {
                LOGD << "Interrupted while waiting for epoll events, retrying";
                continue;
            }

            LOGE << std::format("Failed to wait for epoll events with error {} ({})", err, strerror(err));
            break;
        }

        // Determine which file descriptor(s) became ready and handle them.
        for (std::size_t i = 0; i < std::size_t(numEventsReady); i++) {
            const auto &eventReady = events.at(i);
            if (eventReady.data.fd == netlinkSocketFileDescriptor) {
                HandleNetlinkSocketReady(netlinkSocket);
            } else if (eventReady.data.fd == eventLoopStopFd) {
                stopRequested = true;
            } else {
                LOGW << std::format("Unknown file descriptor {} is ready for reading", eventReady.data.fd);
            }
        }
    }

    LOGI << "Netlink message processing thread has exited";
}

/* static */
void
AccessPointDiscoveryAgentOperationsNetlink::HandleNetlinkSocketReady(NetlinkSocket &netlinkSocket)
{
    LOGD << "Handling netlink socket read availabilty";

    // Read all pending netlink messages.
    for (;;) {
        const int ret = nl_recvmsgs_default(netlinkSocket);
        if (ret < 0) {
            const auto err = errno;
            if (err == EINTR) {
                LOGD << "Interrupted while waiting for netlink messages, retrying";
                continue;
            }

            LOGE << std::format("Failed to receive netlink messages with error {} ({})", err, strerror(err));
            break;
        }

        LOGD << "Successfully processed netlink messages";
        break;
    }
}

// NOLINTEND(concurrency-mt-unsafe)
