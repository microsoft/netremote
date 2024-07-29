
#ifndef ACCESS_POINT_DISCOVERY_AGENT_OPERATIONS_NETLINK_HXX
#define ACCESS_POINT_DISCOVERY_AGENT_OPERATIONS_NETLINK_HXX

#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <stop_token>
#include <thread>
#include <vector>

#include <microsoft/net/netlink/NetlinkSocket.hxx>
#include <microsoft/net/netlink/nl80211/Netlink80211Interface.hxx>
#include <microsoft/net/netlink/nl80211/Netlink80211ProtocolState.hxx>
#include <microsoft/net/wifi/AccessPointLinux.hxx>
#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPointDiscoveryAgentOperations.hxx>
#include <netlink/attr.h>

namespace Microsoft::Net::Wifi
{
// Prototype for obtaining access point attributes.
using GetAccessPointAttributesFunction = std::function<std::optional<AccessPointAttributes>(const std::string &)>;

/**
 * @brief Access point discovery agent operations that use netlink for
 * discovering devices and monitoring device presence.
 *
 * Note that this class is not thread-safe.
 */
struct AccessPointDiscoveryAgentOperationsNetlink :
    public IAccessPointDiscoveryAgentOperations
{
    /**
     * @brief Construct a new Access Point Discovery Agent Operations Netlink object
     *
     * @param accessPointFactory The access point factory to use for creating access points.
     * @param getAccessPointAttributes Function to get access point attributes.
     */
    AccessPointDiscoveryAgentOperationsNetlink(std::shared_ptr<AccessPointFactoryLinux> accessPointFactory, GetAccessPointAttributesFunction getAccessPointAttributes = nullptr);

    ~AccessPointDiscoveryAgentOperationsNetlink() override;

    AccessPointDiscoveryAgentOperationsNetlink(const AccessPointDiscoveryAgentOperationsNetlink &) = delete;
    AccessPointDiscoveryAgentOperationsNetlink &
    operator=(const AccessPointDiscoveryAgentOperationsNetlink &) = delete;
    AccessPointDiscoveryAgentOperationsNetlink(AccessPointDiscoveryAgentOperationsNetlink &&) = delete;
    AccessPointDiscoveryAgentOperationsNetlink &
    operator=(AccessPointDiscoveryAgentOperationsNetlink &&) = delete;

    /**
     * @brief Start the discovery process.
     *
     * @param callback The callback to invoke when an access point is discovered or removed.
     */
    void
    Start(AccessPointPresenceEventCallback accessPointPresenceEventCallback) override;

    /**
     * @brief Stop the discovery process.
     */
    void
    Stop() override;

    /**
     * @brief Perform an asynchronous discovery probe.
     *
     * @return std::future<std::vector<std::shared_ptr<IAccessPoint>>>
     */
    std::future<std::vector<std::shared_ptr<IAccessPoint>>>
    ProbeAsync() override;

private:
    /**
     * @brief Request that the netlink processing loop stop.
     */
    void
    RequestNetlinkProcessingLoopStop() const;

    /**
     * @brief Handles when the netlink socket is ready for reading.
     *
     * @param netlinkSocket The netlink socket that is ready for reading.
     */
    static void
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

    /**
     * @brief Create an access point object for the specified nl80211 interface.
     *
     * @param nl80211Interface The nl80211 interface object to create an access point instance for.
     * @return std::shared_ptr<IAccessPoint>
     */
    std::shared_ptr<IAccessPoint>
    MakeAccessPoint(const Netlink::Nl80211::Nl80211Interface &nl80211Interface);

    /**
     * @brief Find the access point attributes for the specified interface name. This is a simeple wrapper around the
     * provided m_getAccessPointAttributes function.
     *
     * @param interfaceName The name of the interface.
     * @return std::optional<AccessPointAttributes>
     */
    std::optional<AccessPointAttributes>
    GetAccessPointAttributes(const std::string &interfaceName) const;

private:
    std::shared_ptr<AccessPointFactoryLinux> m_accessPointFactory;
    GetAccessPointAttributesFunction m_getAccessPointAttributes;

    // Cookie used to validate that the callback context is valid.
    static constexpr uint32_t CookieValid{ 0x8BADF00DU };
    // Cookie used to invalidate the callback context.
    static constexpr uint32_t CookieInvalid{ 0xDEADBEEFU };

    uint32_t m_cookie{ CookieInvalid };
    AccessPointPresenceEventCallback m_accessPointPresenceCallback{ nullptr };

    int m_eventLoopStopFd{ -1 };
    std::jthread m_netlinkMessageProcessingThread;

    Microsoft::Net::Netlink::Nl80211::Nl80211ProtocolState &m_netlink80211ProtocolState;
};
} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_DISCOVERY_AGENT_OPERATIONS_NETLINK_HXX
