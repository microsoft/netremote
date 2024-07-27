
#ifndef ACCESS_POINT_HXX
#define ACCESS_POINT_HXX

#include <optional>
#include <string>
#include <string_view>

#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPointController.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Base IAccessPoint implementation providing functionality common to all
 * implementations.
 */
struct AccessPoint :
    public IAccessPoint
{
    /**
     * @brief Construct a new AccessPoint object with the given network interface name.
     *
     * @param interfaceName The network interface name representing the access point.
     * @param accessPointControllerFactory The factory used to create controller objects.
     * @param attributes The static attributes of the access point.
     * @param macAddress The mac address of the access point.
     */
    AccessPoint(std::string_view interfaceName, std::shared_ptr<IAccessPointControllerFactory> accessPointControllerFactory, AccessPointAttributes attributes = {}, std::optional<Ieee80211MacAddress> macAddress = std::nullopt);

    /**
     * @brief Get the network interface name representing the access point.
     *
     * @return std::string_view
     */
    std::string_view
    GetInterfaceName() const noexcept override;

    /**
     * @brief Get the mac address of the access point.
     *
     * @return Ieee80211MacAddress
     */
    Ieee80211MacAddress
    GetMacAddress() const noexcept override;

    /**
     * @brief Get the static attributes of an access point.
     *
     * @return AccessPointAttributes&
     */
    const AccessPointAttributes&
    GetAttributes() const noexcept override;

    /**
     * @brief Create a controller object.
     *
     * @return std::unique_ptr<Microsoft::Net::Wifi::IAccessPointController>
     */
    std::unique_ptr<Microsoft::Net::Wifi::IAccessPointController>
    CreateController() override final;

private:
    const std::string m_interfaceName;
    std::shared_ptr<IAccessPointControllerFactory> m_accessPointControllerFactory;
    AccessPointAttributes m_attributes{};
    std::optional<Ieee80211MacAddress> m_macAddress;
};

/**
 * @brief Creates instances of the AccessPoint class.
 */
struct AccessPointFactory :
    public IAccessPointFactory
{
    /**
     * @brief Construct a new Access Point Factory object
     *
     * @param accessPointControllerFactory
     */
    AccessPointFactory(std::shared_ptr<IAccessPointControllerFactory> accessPointControllerFactory);

    /**
     * @brief Create a new access point object for the given network interface with the specified creation arguments.
     *
     * @param interfaceName The name of the interface.
     * @param createArgs Arguments to be passed to the access point during creation.
     * @return std::shared_ptr<IAccessPoint>
     */
    virtual std::shared_ptr<IAccessPoint>
    Create(std::string_view interfaceName, std::unique_ptr<IAccessPointCreateArgs> createArgs) override;

protected:
    /**
     * @brief Get the ControllerFactory object.
     *
     * @return std::shared_ptr<IAccessPointControllerFactory>
     */
    std::shared_ptr<IAccessPointControllerFactory>
    GetControllerFactory() const noexcept;

private:
    std::shared_ptr<IAccessPointControllerFactory> m_accessPointControllerFactory;
};
} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_HXX
