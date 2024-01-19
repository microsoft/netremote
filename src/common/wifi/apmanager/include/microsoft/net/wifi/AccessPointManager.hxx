
#ifndef ACCESS_POINT_MANAGER_HXX
#define ACCESS_POINT_MANAGER_HXX

#include <memory>
#include <mutex>
#include <shared_mutex>
#include <vector>

namespace Microsoft::Net::Wifi
{
struct IAccessPoint;
struct IAccessPointFactory;
struct AccessPointDiscoveryAgent;
enum class AccessPointPresenceEvent;

/**
 * @brief Manages access points which implement the IAccessPoint interface,
 * thereby providing access point services.
 *
 * This class is responsible for tracking the presence of such access points,
 * both when new access points are discovered and when existing access points
 * are removed or become otherwise unavailable.
 */
class AccessPointManager :
    public std::enable_shared_from_this<AccessPointManager>
{
public:
    /**
     * @brief Safely create an instance of the access point manager.
     *
     * @param accessPointFactory
     * @return std::shared_ptr<AccessPointManager>
     */
    [[nodiscard]] static std::shared_ptr<AccessPointManager>
    Create(std::unique_ptr<IAccessPointFactory> accessPointFactory);

    /**
     * @brief Get an instance of this access point manager.
     *
     * @return std::shared_ptr<AccessPointManager>
     */
    std::shared_ptr<AccessPointManager>
    GetInstance() noexcept;

    /**
     * @brief Adds a new access point discovery agent for use.
     *
     * @param discoveryAgent The discovery agent to add.
     */
    void
    AddDiscoveryAgent(std::shared_ptr<AccessPointDiscoveryAgent> discoveryAgent);

    /**
     * @brief Get the IAccessPoint object with the specified interface name.
     *
     * @param interfaceName The interface name of the access point to get.
     * @return std::weak_ptr<IAccessPoint>
     */
    std::weak_ptr<IAccessPoint>
    GetAccessPoint(std::string_view interfaceName) const;

    /**
     * @brief Get a collection of all access points.
     *
     * This function allows the caller to further filter the collection based
     * on its own set of conditions. As such, weak_ptrs are provided since
     * no assumptions are made as to when the caller will actually filter
     * the collection and select a subset for use, thereby promoting the
     * weak pointers to shared pointers.
     *
     * @return std::vector<std::weak_ptr<IAccessPoint>>
     */
    std::vector<std::weak_ptr<IAccessPoint>>
    GetAllAccessPoints() const;

    ~AccessPointManager() = default;
    AccessPointManager(const AccessPointManager&) = delete;
    AccessPointManager(AccessPointManager&&) = delete;
    AccessPointManager&
    operator=(AccessPointManager&) = delete;
    AccessPointManager&
    operator=(AccessPointManager&&) = delete;

protected:
    /**
     * @brief Default constructor.
     *
     * It's intentional that this is *declared* here and default-implemented
     * in the source file. This is required because IAccessPoint
     * and AccessPointDiscoveryAgent are used as incomplete
     * types with std::unique_ptr and std::shared_ptr. In case an exception is
     * thrown in the constructor, their destructors may be called, and the
     * wrapped type must be complete at that time. As such, defining the
     * constructor implementation as default here would require the type to be
     * complete, which is impossible due to the forward declaration.
     * Consequently, the = default implementation is done in the source file
     * instead.
     */
    AccessPointManager(std::unique_ptr<IAccessPointFactory> accessPointFactory);

private:
    /**
     * @brief Callback function for all access point agent presence change events.
     *
     * @param discoveryAgent
     * @param presence
     * @param accessPointChanged
     */
    void
    OnAccessPointPresenceChanged(AccessPointDiscoveryAgent* discoveryAgent, AccessPointPresenceEvent presence, std::shared_ptr<IAccessPoint> accessPointChanged);

    /**
     * @brief Adds a new access point.
     *
     * @param accessPoint The access point to add.
     */
    void
    AddAccessPoint(std::shared_ptr<IAccessPoint> accessPoint);

    /**
     * @brief Removes an existing access point from use.
     *
     * @param accessPoint The access point to remove.
     */
    void
    RemoveAccessPoint(std::shared_ptr<IAccessPoint> accessPoint);

private:
    std::unique_ptr<IAccessPointFactory> m_accessPointFactory;

    mutable std::mutex m_accessPointGate;
    std::vector<std::shared_ptr<IAccessPoint>> m_accessPoints{};

    mutable std::shared_mutex m_discoveryAgentsGate;
    std::vector<std::shared_ptr<AccessPointDiscoveryAgent>> m_discoveryAgents;
};

} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_MANAGER_HXX
