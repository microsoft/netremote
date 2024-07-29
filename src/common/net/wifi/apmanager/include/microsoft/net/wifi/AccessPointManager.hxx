
#ifndef ACCESS_POINT_MANAGER_HXX
#define ACCESS_POINT_MANAGER_HXX

#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include <microsoft/net/wifi/AccessPointAttributes.hxx>

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
     * @param accessPointAttributes The static attributes of the access points to manage.
     * @return std::shared_ptr<AccessPointManager>
     */
    [[nodiscard]] static std::shared_ptr<AccessPointManager>
    Create(std::unordered_map<std::string, AccessPointAttributes> accessPointAttributes = {});

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
     * @return std::optional<std::weak_ptr<IAccessPoint>> 
     */
    std::optional<std::weak_ptr<IAccessPoint>>
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

    /**
     * @brief Get the attributes of the access point with the specified interface name.
     * 
     * @param interfaceName The interface name of the access point to get attributes for.
     * @return std::optional<AccessPointAttributes> 
     */
    std::optional<AccessPointAttributes>
    GetAccessPointAttributes(const std::string& interfaceName) const;

    virtual ~AccessPointManager() = default;
    AccessPointManager(const AccessPointManager&) = delete;
    AccessPointManager(AccessPointManager&&) = delete;
    AccessPointManager&
    operator=(AccessPointManager&) = delete;
    AccessPointManager&
    operator=(AccessPointManager&&) = delete;

protected:
    /**
     * @brief Construct a new AccessPointManager object.
     * 
     * @param accessPointAttributes The static attributes of the access points to manage.
     */
    AccessPointManager(std::unordered_map<std::string, AccessPointAttributes> accessPointAttributes = {});

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
    virtual void
    AddAccessPoint(std::shared_ptr<IAccessPoint> accessPoint);

    /**
     * @brief Removes an existing access point from use.
     *
     * @param accessPoint The access point to remove.
     */
    virtual void
    RemoveAccessPoint(std::shared_ptr<IAccessPoint> accessPoint);

private:
    std::shared_ptr<IAccessPointFactory> m_accessPointFactory;

    mutable std::mutex m_accessPointGate;
    std::vector<std::shared_ptr<IAccessPoint>> m_accessPoints{};

    mutable std::shared_mutex m_discoveryAgentsGate;
    std::vector<std::shared_ptr<AccessPointDiscoveryAgent>> m_discoveryAgents;
    std::unordered_map<std::string, AccessPointAttributes> m_accessPointAttributes{};
};

} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_MANAGER_HXX
