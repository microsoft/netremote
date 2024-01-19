
#ifndef I_ACCESS_POINT_CONTROLLER_HXX
#define I_ACCESS_POINT_CONTROLLER_HXX

#include <exception>
#include <memory>
#include <string_view>

#include <microsoft/net/wifi/Ieee80211AccessPointCapabilities.hxx>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Top-level exception type that may ber thrown by IAccessPointController operations.
 */
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

    /**
     * @brief Get the capabilities of the access point.
     *
     * @return Ieee80211AccessPointCapabilities
     */
    virtual Ieee80211AccessPointCapabilities
    GetCapabilities() = 0;
};

/**
 * @brief Factory for creating IAccessPointController objects.
 */
struct IAccessPointControllerFactory
{
    /**
     * @brief Create a new IAccessPointController object.
     *
     * @return std::unique_ptr<IAccessPointController>
     */
    virtual std::unique_ptr<IAccessPointController>
    Create(std::string_view interfaceName) = 0;
};
} // namespace Microsoft::Net::Wifi

#endif // I_ACCESS_POINT_CONTROLLER_HXX
