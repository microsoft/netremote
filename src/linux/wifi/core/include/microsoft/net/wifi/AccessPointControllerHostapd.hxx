
#ifndef ACCESS_POINT_CONTROLLER_HOSTAPD_HXX
#define ACCESS_POINT_CONTROLLER_HOSTAPD_HXX

#include <string_view>

#include <Wpa/WpaController.hxx>
#include <microsoft/net/wifi/AccessPointController.hxx>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Implementation of IAccessPointController which uses the hostapd daemon to control the access point.
 */
struct AccessPointControllerHostapd :
    public AccessPointController
{
    virtual ~AccessPointControllerHostapd() = default;

    AccessPointControllerHostapd() = delete;

    /**
     * @brief Construct a new AccessPointControllerHostapd object for the specified interface.
     *
     * @param interfaceName The name of the interface to control.
     */
    AccessPointControllerHostapd(std::string_view interfaceName);

    /**
     * @brief Get the Capabilities object
     *
     * @return AccessPointCapabilities2
     */
    virtual AccessPointCapabilities2
    GetCapabilities() override;

private:
    Wpa::WpaController m_wpaController;
};

/**
 * @brief Factory to create AccessPointControllerHostapd objects.
 */
struct AccessPointControllerHostapdFactory :
    public IAccessPointControllerFactory
{
    virtual ~AccessPointControllerHostapdFactory() = default;

    /**
     * @brief Create a new IAccessPointController object.
     *
     * @return std::unique_ptr<IAccessPointController>
     */
    virtual std::unique_ptr<IAccessPointController>
    Create(std::string_view interfaceName) override;
};
} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_CONTROLLER_HOSTAPD_HXX
