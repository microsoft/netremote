
#ifndef ACCESS_POINT_CONTROLLER_HXX
#define ACCESS_POINT_CONTROLLER_HXX

#include <string>
#include <string_view>

#include <microsoft/net/wifi/IAccessPointController.hxx>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Implementation of IAccessPointController for operations that should be
 * common to all implementations.
 */
struct AccessPointController :
    public IAccessPointController
{
    virtual ~AccessPointController() = default;

    /**
     * @brief Construct a new AccessPointController object to control the specified interface.
     * 
     * @param interfaceName 
     */
    AccessPointController(std::string_view interfaceName);

    /**
     * @brief Get the interface name associated with this controller.
     *
     * @return std::string_view
     */
    std::string_view
    GetInterfaceName() const override;

private:
    std::string m_interfaceName;
};
} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_CONTROLLER_HXX
