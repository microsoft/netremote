
#ifndef ACCESS_POINT_LINUX_HXX
#define ACCESS_POINT_LINUX_HXX

#include <memory>
#include <string_view>

#include <microsoft/net/netlink/nl80211/Netlink80211Interface.hxx>
#include <microsoft/net/wifi/AccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPointController.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Access point implementation for Linux.
 */
struct AccessPointLinux :
    public AccessPoint
{
    /**
     * @brief Construct a new AccessPointLinux object with the specified interface name, access point controller
     * factory, and nl80211 interface.
     *
     * @param interfaceName The name of the interface.
     * @param accessPointControllerFactory The access point controller factory to use for creating access point.
     * @param nl80211Interface The nl80211 interface object.
     */
    AccessPointLinux(std::string_view interfaceName, std::shared_ptr<IAccessPointControllerFactory> accessPointControllerFactory, Microsoft::Net::Netlink::Nl80211::Nl80211Interface nl80211Interface);

    /**
     * @brief Get the mac address of the access point.
     *
     * @return Ieee80211MacAddress
     */
    Ieee80211MacAddress
    GetMacAddress() const noexcept override;

private:
    Microsoft::Net::Netlink::Nl80211::Nl80211Interface m_nl80211Interface;
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
    std::shared_ptr<IAccessPoint>
    Create(std::string_view interfaceName) override;

    /**
     * @brief Create a new access point object for the given network interface with the specified creation arguments.
     *
     * @param interfaceName The name of the interface.
     * @param createArgs Arguments to be passed to the access point during creation.
     * @return std::shared_ptr<IAccessPoint>
     */
    std::shared_ptr<IAccessPoint>
    Create(std::string_view interfaceName, std::unique_ptr<IAccessPointCreateArgs> createArgs) override;
};

/**
 * @brief Arguments to be passed to the Linux access point during creation.
 */
struct AccessPointCreateArgsLinux :
    public IAccessPointCreateArgs
{
    /**
     * @brief Construct a new AccessPointCreateArgsLinux object with the specified nl80211 interface.
     *
     * @param nl80211Interface The nl80211 interface object.
     */
    explicit AccessPointCreateArgsLinux(Microsoft::Net::Netlink::Nl80211::Nl80211Interface nl80211Interface);

    Microsoft::Net::Netlink::Nl80211::Nl80211Interface Interface;
};
} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_LINUX_HXX
