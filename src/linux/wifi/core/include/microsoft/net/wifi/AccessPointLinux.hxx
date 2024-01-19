
#ifndef ACCESS_POINT_LINUX_HXX
#define ACCESS_POINT_LINUX_HXX

#include <memory>
#include <string_view>

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

/**
 * @brief Factory to create AccessPointLinux instances.
 */
struct AccessPointFactoryLinux :
    public AccessPointFactory
{
    using AccessPointFactory::AccessPointFactory;

    /**
     * @brief Create a new access point object for the given network interface.
     *
     * @param interfaceName The name of the interface.
     * @return std::shared_ptr<IAccessPoint>
     */
    virtual std::shared_ptr<IAccessPoint>
    Create(std::string_view interfaceName) override;

    /**
     * @brief Create a new access point object for the given network interface with the specified creation arguments.
     *
     * @param interfaceName The name of the interface.
     * @param createArgs Arguments to be passed to the access point during creation.
     * @return std::shared_ptr<IAccessPoint>
     */
    virtual std::shared_ptr<IAccessPoint>
    Create(std::string_view interfaceName, std::unique_ptr<IAccessPointCreateArgs> createArgs) override;
};
} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_LINUX_HXX
