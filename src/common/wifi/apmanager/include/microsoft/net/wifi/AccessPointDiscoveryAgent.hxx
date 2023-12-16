
#ifndef ACCESS_POINT_DISCOVERY_AGENT_HXX
#define ACCESS_POINT_DISCOVERY_AGENT_HXX

#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <shared_mutex>

#include <microsoft/net/wifi/IAccessPointDiscoveryAgentOperations.hxx>

namespace Microsoft::Net::Wifi
{
struct IAccessPoint;

/**
 * @brief Discovers and monitors IAccessPoint instances.
 */
struct AccessPointDiscoveryAgent
{
    /**
     * @brief Destroy the AccessPointDiscoveryAgent object
     */
    virtual ~AccessPointDiscoveryAgent() = default;

    /**
     * @brief Construct a new AccessPointDiscoveryAgent object with the specified operations.
     * 
     * @param operations The discovery operations to use.
     */
    explicit AccessPointDiscoveryAgent(std::unique_ptr<IAccessPointDiscoveryAgentOperations> operations) noexcept;

    /**
     * @brief Register a callback for device presence change events.
     *
     * The caller must ensure the validity of this callback during the lifetime of this object instance.
     *
     * @param onDevicePresenceChanged The callback to register.
     */
    void
    RegisterDiscoveryEventCallback(std::function<void(AccessPointPresenceEvent presence, std::shared_ptr<IAccessPoint> accessPointChanged)> onDevicePresenceChanged);

    /**
     * @brief indicates the started/running state.
     *
     * @return true
     * @return false
     */
    bool
    IsStarted() const noexcept;

    /**
     * @brief Start actively discovering devices.
     */
    void
    Start();

    /**
     * @brief Stop actively discovering devices.
     */
    void
    Stop();

    /**
     * @brief Probe for all existing devices.
     *
     * @return std::future<std::vector<std::shared_ptr<IAccessPoint>>>
     */
    std::future<std::vector<std::shared_ptr<IAccessPoint>>>
    ProbeAsync();

protected:
    /**
     * @brief Wrapper for safely invoking any device presence changed registered callback.
     *
     * @param presence The presence change that occurred.
     * @param accessPointChanged The device the change occurred for.
     */
    void
    DevicePresenceChanged(AccessPointPresenceEvent presence, std::shared_ptr<IAccessPoint> accessPointChanged) const noexcept;

private:
    std::unique_ptr<IAccessPointDiscoveryAgentOperations> m_operations;
    std::atomic<bool> m_started{ false };

    mutable std::shared_mutex m_onDevicePresenceChangedGate;
    std::function<void(AccessPointPresenceEvent presence, std::shared_ptr<IAccessPoint> accessPointChanged)> m_onDevicePresenceChanged;
};

} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_DISCOVERY_AGENT_HXX
