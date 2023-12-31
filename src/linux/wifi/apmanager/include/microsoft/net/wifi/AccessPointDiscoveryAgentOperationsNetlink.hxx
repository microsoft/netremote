
#ifndef ACCESS_POINT_DISCOVERY_AGENT_OPERATIONS_NETLINK_HXX
#define ACCESS_POINT_DISCOVERY_AGENT_OPERATIONS_NETLINK_HXX

#include <cstdint>
#include <stop_token>
#include <string>
#include <thread>
#include <unordered_map>

#include <linux/nl80211.h>
#include <microsoft/net/netlink/NetlinkMessage.hxx>
#include <microsoft/net/netlink/NetlinkSocket.hxx>
#include <microsoft/net/wifi/IAccessPointDiscoveryAgentOperations.hxx>
#include <netlink/netlink.h>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Access point discovery agent operations that use netlink for
 * discovering devices and monitoring device presence.
 *
 * Note that this class is not thread-safe.
 */
struct AccessPointDiscoveryAgentOperationsNetlink :
    public IAccessPointDiscoveryAgentOperations
{
    AccessPointDiscoveryAgentOperationsNetlink();

    virtual ~AccessPointDiscoveryAgentOperationsNetlink();

    void
    Start(AccessPointPresenceEventCallback accessPointPresenceEventCallback) override;

    void
    Stop() override;

    std::future<std::vector<std::string>>
    ProbeAsync() override;

private:
    /**
     * @brief Request that the netlink processing loop stop.
     */
    void
    RequestNetlinkProcessingLoopStop();

    /**
     * @brief Handles when the netlink socket is ready for reading.
     *
     * @param netlinkSocket The netlink socket that is ready for reading.
     */
    void
    HandleNetlinkSocketReady(Microsoft::Net::Netlink::NetlinkSocket &netlinkSocket);

    /**
     * @brief Thread function for processing netlink messages.
     *
     * @param netlinkSocket The netlink socket to use for processing messages.
     * @param stopToken The stop token to use for stopping the thread.
     */
    void
    ProcessNetlinkMessagesThread(Microsoft::Net::Netlink::NetlinkSocket netlinkSocket, std::stop_token stopToken);

    /**
     * @brief C-style function for use with netlink message callbacks.
     *
     * @param netlinkMessage The netlink message being processed.
     * @param contextArgument The callback context argument. This must be a
     * pointer to the class instance (AccessPointDiscoveryAgentOperationsNetlink*)
     * which owns the callback.
     * @return int
     */
    static int
    ProcessNetlinkMessagesCallback(struct nl_msg *netlinkMessage, void *contextArgument);

    /**
     * @brief Process a single netlink message.
     *
     * @param netlinkMessage The netlink message to process.
     * @param accessPointPresenceEventCallback The callback to invoke when an access point presence event occurs.
     */
    int
    ProcessNetlinkMessage(struct nl_msg *netlinkMessage, AccessPointPresenceEventCallback &accessPointPresenceEventCallback);

private:
    // Cookie used to validate that the callback context is valid.
    static constexpr uint32_t CookieValid{ 0x8BADF00Du };
    // Cookie used to invalidate the callback context.
    static constexpr uint32_t CookieInvalid{ 0xDEADBEEFu };

    int m_nl80211NetlinkId{ -1 };
    int m_nl80211MulticastGroupIdConfig{ -1 };

    uint32_t m_cookie{ CookieInvalid };
    AccessPointPresenceEventCallback m_accessPointPresenceCallback{ nullptr };

    int m_eventLoopStopFd{ -1 };
    std::jthread m_netlinkMessageProcessingThread;

    struct WifiInterfaceInfo
    {
        std::string Name;
        nl80211_iftype Type;
    };

    std::unordered_map<int, WifiInterfaceInfo> m_interfaceInfo;
};
} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_DISCOVERY_AGENT_OPERATIONS_NETLINK_HXX
