
#ifndef ACCESS_POINT_LINUX_HXX
#define ACCESS_POINT_LINUX_HXX

#include <memory>

#include <microsoft/net/wifi/AccessPoint.hxx>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Access point implementation for Linux.
 */
struct AccessPointLinux :
    public AccessPoint
{
    /**
     * @brief Inherit the constructors from the base class.
     */
    using AccessPoint::AccessPoint;

    virtual std::unique_ptr<Microsoft::Net::Wifi::IAccessPointController>
    CreateController() override;
};

} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_LINUX_HXX
