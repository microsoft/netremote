
#ifndef I_ACCESS_POINT_DISCOVERY_AGENT_OPERATIONS_HXX
#define I_ACCESS_POINT_DISCOVERY_AGENT_OPERATIONS_HXX

#include <functional>
#include <future>
#include <memory>
#include <vector>

namespace Microsoft::Net::Wifi
{
enum class AccessPointPresenceEvent {
    Arrived,
    Departed,
};

struct IAccessPoint;

/**
 * @brief Prototype for the callback invoked when an access point is discovered or removed.
 */
using AccessPointPresenceEventCallback = std::function<void(AccessPointPresenceEvent, std::shared_ptr<IAccessPoint>)>;

/**
 * @brief Operations used to perform discovery of access points, used by
 * AccessPointDiscoveryAgent as part of the strategy design pattern.
 */
struct IAccessPointDiscoveryAgentOperations
{
    virtual ~IAccessPointDiscoveryAgentOperations() = default;

    /**
     * @brief Start the discovery process.
     * 
     * @param callback The callback to invoke when an access point is discovered or removed.
     */
    virtual void
    Start(AccessPointPresenceEventCallback callback) = 0;

    /**
     * @brief Stop the discovery process.
     */
    virtual void
    Stop() = 0;

    /**
     * @brief Perform an asynchronous discovery probe.
     *
     * @return std::future<std::vector<std::shared_ptr<IAccessPoint>>>
     */
    virtual std::future<std::vector<std::shared_ptr<IAccessPoint>>>
    ProbeAsync() = 0;
};

} // namespace Microsoft::Net::Wifi

#endif // I_ACCESS_POINT_DISCOVERY_AGENT_OPERATIONS_HXX
