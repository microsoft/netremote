
#ifndef ACCESS_POINT_MANAGER_HXX
#define ACCESS_POINT_MANAGER_HXX

#include <string>
#include <unordered_map>

#include <microsoft/net/wifi/AccessPoint.hxx>

namespace Microsoft::Net::Wifi
{
/**
 * @brief The AccessPointManager class is responsible for managing access points
 * including creation, destruction, and configuration.
 */
struct AccessPointManager
{
    /**
     * @brief Return a list of the currently available access points.
     * 
     * @return std::unordered_map<std::string, AccessPoint> 
     */
    std::unordered_map<std::string, AccessPoint> EnumerateAccessPoints() const noexcept;

private:
    std::unordered_map<std::string, AccessPoint> m_accessPoints;
};
} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_MANAGER_HXX
