
#ifndef ACCESS_POINT_MANAGER_TEST
#define ACCESS_POINT_MANAGER_TEST

#include <memory>

#include <microsoft/net/wifi/AccessPointManager.hxx>
#include <microsoft/net/wifi/IAccessPoint.hxx>

namespace Microsoft::Net::Wifi::Test
{
/**
 * @brief AccessPointManager to be used in tests, allowing access to protected methods to add/remove known access
 * points.
 */
struct AccessPointManagerTest :
    public Microsoft::Net::Wifi::AccessPointManager
{
    /**
     * @brief Construct a new AccessPointManagerTest object.
     */
    AccessPointManagerTest() = default;

    /**
     * @brief Adds a new access point.
     *
     * @param accessPoint The access point to add.
     */
    void
    AddAccessPoint(std::shared_ptr<IAccessPoint> accessPoint) override;

    /**
     * @brief Removes an existing access point from use.
     *
     * @param accessPoint The access point to remove.
     */
    void
    RemoveAccessPoint(std::shared_ptr<IAccessPoint> accessPoint) override;
};
} // namespace Microsoft::Net::Wifi::Test

#endif // ACCESS_POINT_MANAGER_TEST
