
#include <cerrno>
#include <format>

#include <linux/rtnetlink.h>
#include <microsoft/net/wifi/AccessPointDiscoveryAgentOperationsNetlink.hxx>
#include <notstd/Scope.hxx>
#include <plog/Log.h>

using namespace Microsoft::Net::Wifi;

void
AccessPointDiscoveryAgentOperationsNetlink::Start(AccessPointPresenceEventCallback callback)
{
    // Don't start a new thread if one is already running.
    if (m_netlinkThread.joinable()) {
        return;
    }

    // Open a new netlink socket with the routing/networking family group.
    auto* netlinkSocket = mnl_socket_open(NETLINK_ROUTE);
    if (netlinkSocket == nullptr) {
        // TODO: this function needs to signal the error either through its return type, or an exception.
        const auto err = errno;
        LOG_ERROR << std::format("Failed to open netlink socket with error %d (%s)", err, strerror(err));
        return;
    }

    auto closeSocketOnFailure = notstd::ScopeExit([&netlinkSocket]() {
        mnl_socket_close(netlinkSocket);
    });

    // Find the socket to the link group of messages.
    const auto bindResult = mnl_socket_bind(netlinkSocket, RTMGRP_LINK, MNL_SOCKET_AUTOPID);
    if (bindResult < 0) {
        const auto err = errno;
        LOG_ERROR << std::format("Failed to bind netlink socket with error %d (%s)", err, strerror(err));
        return;
    }

    closeSocketOnFailure.release();

    // Start a thread to process netlink messages.
    m_netlinkThread = std::jthread([this, netlinkSocket, callback = std::move(callback)](std::stop_token stopToken) {
        ProcessNetlinkMessages(netlinkSocket, std::move(callback), stopToken);
    });
}

void
AccessPointDiscoveryAgentOperationsNetlink::Stop()
{
    if (m_netlinkThread.joinable()) {
        m_netlinkThread.request_stop();
        m_netlinkThread.join();
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

void
AccessPointDiscoveryAgentOperationsNetlink::ProcessNetlinkMessages([[maybe_unused]] mnl_socket* netlinkSocket, [[maybe_unused]] AccessPointPresenceEventCallback callback, [[maybe_unused]] std::stop_token stopToken)
{
    // TODO:
}
