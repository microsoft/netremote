
#ifndef ACCESS_POINT_CONTROLLER_HXX
#define ACCESS_POINT_CONTROLLER_HXX

#include <string>
#include <string_view>

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

    AccessPointController(std::string_view interfaceName);

    std::string_view
    GetInterfaceName() const noexcept override;

private:
    std::string m_interfaceName;
};
} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_CONTROLLER_HXX
