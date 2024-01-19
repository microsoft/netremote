
#ifndef I_ACCESS_POINT_CONTROLLER_HXX
#define I_ACCESS_POINT_CONTROLLER_HXX

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
};
} // namespace Microsoft::Net::Wifi

#endif // I_ACCESS_POINT_CONTROLLER_HXX
