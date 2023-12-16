
#ifndef ACCESS_POINT_DISCOVERY_AGENT_HXX
#define ACCESS_POINT_DISCOVERY_AGENT_HXX

#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <shared_mutex>

namespace Microsoft::Net::Wifi
{
struct IAccessPoint;

/**
 * @brief The presence of a an access point.
 */
enum class AccessPointPresenceEvent 
{
    Arrived,
    Departed,
};

/**
 * @brief Discovers and monitors IAccessPoint instances.
 */
struct AccessPointDiscoveryAgent
{
    /**
     * @brief Construct a new AccessPointDiscoveryAgent object.
     */
    AccessPointDiscoveryAgent() = default;

    /**
     * @brief Destroy the AccessPointDiscoveryAgent object
     */
    virtual ~AccessPointDiscoveryAgent() = default;

    /**
     * @brief Construct a new AccessPointDiscoveryAgent object.
     *
     * @param onDevicePresenceChanged
     */
    explicit AccessPointDiscoveryAgent(std::function<void(AccessPointPresenceEvent presence, std::shared_ptr<IAccessPoint> accessPointChanged)> onDevicePresenceChanged);

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
    bool IsStarted() const noexcept;

    /**
     * @brief Start actively discovering devices.
     */
    void Start();

    /**
     * @brief Stop actively discovering devices.
     */
    void Stop();

    /**
     * @brief Probe for all existing devices.
     * 
     * @return std::future<std::vector<std::shared_ptr<IAccessPoint>>> 
     */
    std::future<std::vector<std::shared_ptr<IAccessPoint>>> ProbeAsync();

protected:
    /**
     * @brief Wrapper for safely invoking any device presence changed registered callback.
     *
     * @param presence The presence change that occurred.
     * @param accessPointChanged The device the change occurred for.
     */
    void DevicePresenceChanged(AccessPointPresenceEvent presence, std::shared_ptr<IAccessPoint> accessPointChanged) const noexcept;

protected:
    /**
     * @brief Derived class implementation of discovery start.
     */
    virtual void StartImpl();

    /**
     * @brief Derived class implementation of discovery stop.
     */
    virtual void StopImpl();

    /**
     * @brief Derived class implementation of asynchronous discovery probe.
     *
     * @return std::future<std::vector<std::shared_ptr<IAccessPoint>>>
     */
    virtual std::future<std::vector<std::shared_ptr<IAccessPoint>>> ProbeAsyncImpl();

private:
    std::atomic<bool> m_started{ false };

    mutable std::shared_mutex m_onDevicePresenceChangedGate;
    std::function<void(AccessPointPresenceEvent presence, std::shared_ptr<IAccessPoint> accessPointChanged)> m_onDevicePresenceChanged;
};

} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_DISCOVERY_AGENT_HXX
