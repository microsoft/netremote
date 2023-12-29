
#include <cerrno>
#include <format>
#include <stdexcept>

#include <microsoft/net/wifi/AccessPointDiscoveryAgentOperationsNetlink.hxx>
#include <netlink/handlers.h>
#include <plog/Log.h>

using namespace Microsoft::Net::Wifi;

using Microsoft::Net::Netlink::NetlinkMessage;

AccessPointDiscoveryAgentOperationsNetlink::AccessPointDiscoveryAgentOperationsNetlink() :
    m_cookie(CookieValid)
{}

AccessPointDiscoveryAgentOperationsNetlink::~AccessPointDiscoveryAgentOperationsNetlink()
{
    // Invalidate the cookie as a rudimentary way to detect use-after-free
    // within the netlink thread.
    m_cookie = CookieInvalid;
}

void
AccessPointDiscoveryAgentOperationsNetlink::Start(AccessPointPresenceEventCallback accessPointPresenceEventCallback)
{
    using Microsoft::Net::Netlink::NetlinkSocket;

    // Open a new netlink socket with the routing/networking family group.
    auto netlinkSocket{ NetlinkSocket::Allocate() };
    if (netlinkSocket == nullptr) {
        // TODO: this function needs to signal the error either through its return type, or an exception.
        const auto err = errno;
        LOG_ERROR << std::format("Failed to allocate new netlink socket with error %d (%s)", err, strerror(err));
        return;
    }

    // Register a callback to be invoked for all valid netlink messages (ie.
    // those that have been successfuly parsed).
    int ret = nl_socket_modify_cb(netlinkSocket, NL_CB_VALID, NL_CB_CUSTOM, ProcessNetlinkMessagesCallback, this);
    if (ret < 0) {
        const auto err = errno;
        LOG_ERROR << std::format("Failed to modify netlink socket callback with error %d (%s)", err, strerror(err));
        return;
    }

    // Subscribe to the link group of messages.
    ret = nl_socket_add_membership(netlinkSocket, RTMGRP_LINK);
    if (ret < 0) {
        const auto err = errno;
        LOG_ERROR << std::format("Failed to add netlink socket membership with error %d (%s)", err, strerror(err));
        return;
    }

    // Update the acces point presence callback for the netlink message handler to use.
    // Note: This is not thread-safe.
    m_accessPointPresenceCallback = std::move(accessPointPresenceEventCallback);
    m_netlinkSocket = std::move(netlinkSocket);
}

void
AccessPointDiscoveryAgentOperationsNetlink::Stop()
{
    if (m_netlinkSocket != nullptr)
    {
        nl_socket_free(m_netlinkSocket);
        m_netlinkSocket = nullptr; 
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
AccessPointDiscoveryAgentOperationsNetlink::ProcessNetlinkMessage([[maybe_unused]] NetlinkMessage& netlinkMessage, [[maybe_unused]] AccessPointPresenceEventCallback &accessPointPresenceEventCallback)
{
    // TODO:
    return NL_OK;
}

/* static */
int
AccessPointDiscoveryAgentOperationsNetlink::ProcessNetlinkMessagesCallback(struct nl_msg *netlinkMessageRaw, void *contextArgument)
{
    auto *instance{ static_cast<AccessPointDiscoveryAgentOperationsNetlink *>(contextArgument) };
    if (instance == nullptr) {
        static constexpr auto errorMessage{ "Netlink message callback context is null; this is a bug!" };
        LOG_FATAL << errorMessage;
        throw std::runtime_error(errorMessage);
    } else if (instance->m_cookie != CookieValid) {
        LOG_ERROR << "Netlink message callback context cookie is invalid; discovery agent instance has been destroyed";
        return NL_STOP;
    }

    NetlinkMessage netlinkMessage{ netlinkMessageRaw };
    auto ret = instance->ProcessNetlinkMessage(netlinkMessage, instance->m_accessPointPresenceCallback);
    LOG_VERBOSE << std::format("Processed netlink message with result %d", ret);

    return ret;
}
