
#ifndef I_ACCESS_POINT_HXX
#define I_ACCESS_POINT_HXX

#include <memory>
#include <string_view>

#include <microsoft/net/wifi/IAccessPointController.hxx>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Represents a wireless access point.
 */
struct IAccessPoint
{
    /**
     * @brief Destroy the IAccessPoint object.
     */
    virtual ~IAccessPoint() = default;

    /**
     * @brief Get the network interface name representing the access point.
     *
     * @return std::string_view
     */
    virtual std::string_view
    GetInterfaceName() const = 0;

    /**
     * @brief Create a new instance that can control the access point.
     *
     * @return std::unique_ptr<IAccessPointController>
     */
    virtual std::unique_ptr<IAccessPointController>
    CreateController() = 0;
};

/**
 * @brief Arguments to be passed to the Create method of IAccessPointFactory.
 */
struct IAccessPointCreateArgs 
{
    virtual ~IAccessPointCreateArgs() = default;
};

/**
 * @brief Interface for creating access points.
 */
struct IAccessPointFactory
{
    /**
     * @brief Destroy the IAccessPointFactory object.
     */
    virtual ~IAccessPointFactory() = default;

    /**
     * @brief Create a new access point object for the given network interface.
     * 
     * @param interfaceName The name of the interface.
     * @return std::shared_ptr<IAccessPoint> 
     */
    virtual std::shared_ptr<IAccessPoint>
    Create(std::string_view interfaceName) = 0;

    /**
     * @brief Create a new access point object for the given network interface with the specified creation arguments.
     * 
     * @param interfaceName The name of the interface.
     * @param createArgs Arguments to be passed to the access point during creation.
     * @return std::shared_ptr<IAccessPoint> 
     */
    virtual std::shared_ptr<IAccessPoint>
    Create(std::string_view interfaceName, std::unique_ptr<IAccessPointCreateArgs> createArgs) = 0;
};
} // namespace Microsoft::Net::Wifi

#endif // I_ACCESS_POINT_HXX
