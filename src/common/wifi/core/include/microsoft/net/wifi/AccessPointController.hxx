
#ifndef ACCESS_POINT_CONTROLLER_HXX
#define ACCESS_POINT_CONTROLLER_HXX

#include <memory>

#include <microsoft/net/wifi/IAccessPointController.hxx>

namespace Microsoft::Net::Wifi
{
struct IAccessPoint;

/**
 * @brief Implementation of IAccessPointController for operations that should be
 * common to all implementations.
 */
struct AccessPointController :
    public IAccessPointController
{
    virtual ~AccessPointController() = default;

    AccessPointController(std::weak_ptr<IAccessPoint> accessPointWeak);

    virtual std::weak_ptr<Microsoft::Net::Wifi::IAccessPoint>
    GetAccessPoint() const noexcept override;

private:
    std::weak_ptr<Microsoft::Net::Wifi::IAccessPoint> m_accessPointWeak;
};
} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_CONTROLLER_HXX
