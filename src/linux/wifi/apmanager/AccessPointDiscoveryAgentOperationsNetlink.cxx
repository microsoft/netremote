
#include <cerrno>
#include <format>
#include <stdexcept>

#include <linux/if.h>
#include <linux/if_link.h>
#include <linux/nl80211.h>
#include <linux/rtnetlink.h>
#include <magic_enum.hpp>
#include <microsoft/net/wifi/AccessPointDiscoveryAgentOperationsNetlink.hxx>
#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <netlink/genl/ctrl.h>
#include <netlink/genl/genl.h>
#include <netlink/handlers.h>
#include <notstd/Scope.hxx>
#include <plog/Log.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/types.h>
#include <unistd.h>

using namespace Microsoft::Net::Wifi;

using Microsoft::Net::Netlink::NetlinkMessage;
using Microsoft::Net::Netlink::NetlinkSocket;

AccessPointDiscoveryAgentOperationsNetlink::AccessPointDiscoveryAgentOperationsNetlink() :
    m_cookie(CookieValid)
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

    // Allocate a new netlink socket.
    auto netlinkSocket{ NetlinkSocket::Allocate() };
    if (netlinkSocket == nullptr) {
        LOG_ERROR << "Failed to allocate new netlink socket for nl control";
        return;
    }

    // Connect the socket to the generic netlink family.
    int ret = genl_connect(netlinkSocket);
    if (ret < 0) {
        const auto err = errno;
        LOG_ERROR << std::format("Failed to connect netlink socket for nl control with error {} ({})", err, strerror(err));
        return;
    }

    // Lookup the nl80211 netlink id if not already done.
    int nl80211NetlinkId = m_nl80211NetlinkId;
    if (nl80211NetlinkId == -1) {
        nl80211NetlinkId = genl_ctrl_resolve(netlinkSocket, NL80211_GENL_NAME);
        if (nl80211NetlinkId < 0) {
            LOG_ERROR << std::format("Failed to resolve nl80211 netlink id with error {}", nl80211NetlinkId);
            return;
        }
        m_nl80211NetlinkId = nl80211NetlinkId;
    }

    // Lookup the membership id for the "config" multicast group if not already done.
    int nl80211MulticastGroupIdConfig = m_nl80211MulticastGroupIdConfig;
    if (nl80211MulticastGroupIdConfig == -1) {
        nl80211MulticastGroupIdConfig = genl_ctrl_resolve_grp(netlinkSocket, NL80211_GENL_NAME, NL80211_MULTICAST_GROUP_CONFIG);
        if (nl80211MulticastGroupIdConfig < 0) {
            LOG_ERROR << std::format("Failed to resolve nl80211 multicast group id for config with error {} ({})", nl80211MulticastGroupIdConfig, nl_geterror(nl80211MulticastGroupIdConfig));
            return;
        }
        m_nl80211MulticastGroupIdConfig = nl80211MulticastGroupIdConfig;
    }

    // Subscribe to configuration messages.
    ret = nl_socket_add_membership(netlinkSocket, nl80211MulticastGroupIdConfig);
    if (ret < 0) {
        const auto err = errno;
        LOG_ERROR << std::format("Failed to add netlink socket membership for '" NL80211_MULTICAST_GROUP_CONFIG "' group with error {} ({})", err, strerror(err));
        return;
    }

    // Update the access point presence callback for the netlink message handler to use.
    // Note: This is not thread-safe.
    m_accessPointPresenceCallback = std::move(accessPointPresenceEventCallback);
    m_netlinkMessageProcessingThread = std::jthread([this, netlinkSocket = std::move(netlinkSocket)](std::stop_token stopToken) mutable {
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

std::future<std::vector<std::shared_ptr<IAccessPoint>>>
AccessPointDiscoveryAgentOperationsNetlink::ProbeAsync()
{
    std::promise<std::vector<std::shared_ptr<IAccessPoint>>> probePromise{};
    auto probeFuture = probePromise.get_future();

    // TODO: implement this.
    std::vector<std::shared_ptr<IAccessPoint>> accessPoints{};
    probePromise.set_value(std::move(accessPoints));

    return probeFuture;
}

int
AccessPointDiscoveryAgentOperationsNetlink::ProcessNetlinkMessage(struct nl_msg *netlinkMessage, AccessPointPresenceEventCallback &accessPointPresenceEventCallback)
{
    std::shared_ptr<IAccessPoint> accessPoint{ nullptr };
    AccessPointPresenceEvent accessPointPresenceEvent;
    auto netlinkMessageHeader{ nlmsg_hdr(netlinkMessage) };

    if (netlinkMessageHeader == nullptr) {
        LOG_ERROR << "Netlink message header is null, ignoring message";
        return NL_SKIP;
    }

    auto interfaceNameAttribute = nlmsg_find_attr(netlinkMessageHeader, sizeof *netlinkMessageHeader, IFLA_IFNAME);
    if (interfaceNameAttribute == nullptr) {
        LOG_ERROR << "Netlink message does not contain interface name attribute, ignoring message";
        return NL_SKIP;
    }

    const auto *interfaceName = static_cast<const char *>(RTA_DATA(interfaceNameAttribute));
    const auto *interfaceInfoMessage{ static_cast<const struct ifinfomsg *>(NLMSG_DATA(netlinkMessageHeader)) };
    LOG_VERBOSE << std::format("Received netlink message with type {}, interface {}, index {}", netlinkMessageHeader->nlmsg_type, interfaceName, interfaceInfoMessage->ifi_index);

    switch (netlinkMessageHeader->nlmsg_type) {
    case RTM_NEWLINK:
        accessPointPresenceEvent = AccessPointPresenceEvent::Arrived;
        // TODO: process message
        break;
    case RTM_DELLINK:
        accessPointPresenceEvent = AccessPointPresenceEvent::Departed;
        // TODO: process message
        break;
    default:
        PLOG_VERBOSE << std::format("Ignoring netlink message with type {}", netlinkMessageHeader->nlmsg_type);
        return NL_SKIP;
    }

    if (accessPointPresenceEventCallback != nullptr) {
        LOG_VERBOSE << std::format("Invoking access point presence event callback with event args 'presence={}, accessPointChanged={}'", magic_enum::enum_name(accessPointPresenceEvent), accessPoint != nullptr ? accessPoint->GetInterface() : "<unknown>");
        accessPointPresenceEventCallback(accessPointPresenceEvent, std::move(accessPoint));
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
