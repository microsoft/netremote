
#ifndef I_ACCESS_POINT_CONTROLLER_HXX
#define I_ACCESS_POINT_CONTROLLER_HXX

#include <memory>

namespace Microsoft::Net::Wifi
{
struct IAccessPoint;

/**
 * @brief Class allowing control of an access point.
 */
struct IAccessPointController
{
    /**
     * @brief Destroy the IAccessPointController object.
     */
    virtual ~IAccessPointController() = default;

    /**
     * @brief Get the access point this object controls.
     * 
     * @return std::weak_ptr<IAccessPoint> 
     */
    virtual std::weak_ptr<IAccessPoint>
    GetAccessPoint() const noexcept = 0;
};
} // namespace Microsoft::Net::Wifi

#endif // I_ACCESS_POINT_CONTROLLER_HXX
