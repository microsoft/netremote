
#ifndef I_ACCESS_POINT_DISCOVERY_AGENT_OPERATIONS_HXX
#define I_ACCESS_POINT_DISCOVERY_AGENT_OPERATIONS_HXX

#include <future>
#include <memory>
#include <vector>

namespace Microsoft::Net::Wifi
{
struct IAccessPoint;

/**
 * @brief Operations used to perform discovery of access points, used by
 * AccessPointDiscoveryAgent as part of the strategy design pattern.
 */
struct IAccessPointDiscoveryAgentOperations
{
    virtual ~IAccessPointDiscoveryAgentOperations() = default;

    /**
     * @brief Start the discovery process.
     */
    virtual void
    Start() = 0;

    /**
     * @brief Stop the discovery process.
     */
    virtual void
    Stop() = 0;

    /**
     * @brief Derived class implementation of asynchronous discovery probe.
     *
     * @return std::future<std::vector<std::shared_ptr<IAccessPoint>>>
     */
    virtual std::future<std::vector<std::shared_ptr<IAccessPoint>>>
    ProbeAsync() = 0;
};

} // namespace Microsoft::Net::Wifi

#endif // I_ACCESS_POINT_DISCOVERY_AGENT_OPERATIONS_HXX
