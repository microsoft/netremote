
#ifndef ACCESS_POINT_MANAGER_HXX
#define ACCESS_POINT_MANAGER_HXX

#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>

#include <microsoft/net/wifi/AccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPointFactory.hxx>

namespace Microsoft::Net::Wifi
{
/**
 * @brief The AccessPointManager class is responsible for managing access points
 * including creation, destruction, and configuration.
 */
struct AccessPointManager
{
    /**
     * @brief Construct a new AccessPointManager with the given access point factory.
     * 
     * @param accessPointFactory 
     */
    AccessPointManager(std::unique_ptr<IAccessPointFactory> accessPointFactory);

    /**
     * @brief Return a list of the currently available access points.
     * 
     * @return std::unordered_map<std::string, std::weak_ptr<AccessPoint>> 
     */
    std::unordered_map<std::string, std::weak_ptr<AccessPoint>> EnumerateAccessPoints() const noexcept;

private:
    std::unique_ptr<IAccessPointFactory> m_accessPointFactory;

    mutable std::shared_mutex m_accessPointsGate;
    std::unordered_map<std::string, std::shared_ptr<AccessPoint>> m_accessPoints;
};
} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_MANAGER_HXX
