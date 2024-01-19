
#ifndef I_ACCESS_POINT_CONTROLLER_HXX
#define I_ACCESS_POINT_CONTROLLER_HXX

#include <exception>
#include <string_view>

#include <microsoft/net/wifi/AccessPointCapabilities.hxx>

namespace Microsoft::Net::Wifi
{
struct IAccessPoint;

struct AccessPointControllerException :
    public std::exception
{
    using std::exception::exception;
};

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
     * @brief Get the interface name associated with this controller.
     *
     * @return std::string_view
     */
    virtual std::string_view
    GetInterfaceName() const noexcept = 0;
};
} // namespace Microsoft::Net::Wifi

#endif // I_ACCESS_POINT_CONTROLLER_HXX
