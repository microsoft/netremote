
#ifndef ACCESS_POINT_DISCOVERY_AGENT_HXX
#define ACCESS_POINT_DISCOVERY_AGENT_HXX

#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <shared_mutex>
#include <string>

#include <microsoft/net/wifi/IAccessPointDiscoveryAgentOperations.hxx>

namespace Microsoft::Net::Wifi
{
struct IAccessPoint;

/**
 * @brief Discovers and monitors IAccessPoint instances.
 */
struct AccessPointDiscoveryAgent :
    public std::enable_shared_from_this<AccessPointDiscoveryAgent>
{
    /**
     * @brief Destroy the AccessPointDiscoveryAgent object
     */
    virtual ~AccessPointDiscoveryAgent();

    /**
     * @brief Construct a new AccessPointDiscoveryAgent object with the specified operations.
     *
     * @param operations The discovery operations to use.
     * @return AccessPointDiscoveryAgent
     */
    [[nodiscard]] static std::shared_ptr<AccessPointDiscoveryAgent>
    Create(std::unique_ptr<IAccessPointDiscoveryAgentOperations> operations);

    /**
     * @brief Get a shared pointer reference of this object instance.
     *
     * @return std::shared_ptr<AccessPointDiscoveryAgent>
     */
    std::shared_ptr<AccessPointDiscoveryAgent>
    GetInstance() noexcept;

    /**
     * @brief Register a callback for device presence change events.
     *
     * The caller must ensure the validity of this callback during the lifetime of this object instance.
     *
     * @param onDevicePresenceChanged The callback to register.
     */
    void
    RegisterDiscoveryEventCallback(AccessPointPresenceEventCallback onDevicePresenceChanged);

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
     * @return std::future<std::vector<std::string>>
     */
    std::future<std::vector<std::string>>
    ProbeAsync();

    /**
     * @brief Perform an asynchronous discovery probe.
     *
     * @return std::future<std::vector<std::shared_ptr<IAccessPoint>>>
     */
    std::future<std::vector<std::shared_ptr<IAccessPoint>>>
    ProbeAsync2();

protected:
    /**
     * @brief Construct a new AccessPointDiscoveryAgent object with the specified operations.
     *
     * @param operations The discovery operations to use.
     */
    explicit AccessPointDiscoveryAgent(std::unique_ptr<IAccessPointDiscoveryAgentOperations> operations) noexcept;

    /**
     * @brief Wrapper for safely invoking any device presence changed registered callback.
     *
     * @param presence The presence change that occurred.
     * @param accessPoint The access point instance that changed.
     */
    void
    DevicePresenceChanged(AccessPointPresenceEvent presence, std::shared_ptr<IAccessPoint> accessPoint) const noexcept;

private:
    std::unique_ptr<IAccessPointDiscoveryAgentOperations> m_operations;
    std::atomic<bool> m_started{ false };

    mutable std::shared_mutex m_onDevicePresenceChangedGate;
    AccessPointPresenceEventCallback m_onDevicePresenceChanged;
};

} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_DISCOVERY_AGENT_HXX
