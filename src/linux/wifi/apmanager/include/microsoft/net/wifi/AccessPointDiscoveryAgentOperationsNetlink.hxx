
#ifndef ACCESS_POINT_DISCOVERY_AGENT_OPERATIONS_NETLINK_HXX
#define ACCESS_POINT_DISCOVERY_AGENT_OPERATIONS_NETLINK_HXX

#include <microsoft/net/wifi/IAccessPointDiscoveryAgentOperations.hxx>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Access point discovery agent operations that use netlink for
 * discovering devices and monitoring device presence.
 */
struct AccessPointDiscoveryAgentOperationsNetlink :
    public IAccessPointDiscoveryAgentOperations
{
    void
    Start(AccessPointPresenceEventCallback callback) override;

    void
    Stop() override;

    std::future<std::vector<std::shared_ptr<IAccessPoint>>>
    ProbeAsync() override;
};
} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_DISCOVERY_AGENT_OPERATIONS_NETLINK_HXX
