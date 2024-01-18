
#ifndef ACCESS_POINT_LINUX_HXX
#define ACCESS_POINT_LINUX_HXX

#include <memory>
#include <string_view>

#include <microsoft/net/wifi/AccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPointFactory.hxx>

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

/**
 * @brief IAccessPoint factory for Linux.
 */
struct AccessPointFactoryLinux :
    public IAccessPointFactory
{
    /**
     * @brief Create an access point object for the given network interface.
     *
     * @param interface
     * @return std::shared_ptr<IAccessPoint>
     */
    std::shared_ptr<IAccessPoint>
    Create(std::string_view interface) override;
};
} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_LINUX_HXX
