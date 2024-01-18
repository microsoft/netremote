
#ifndef ACCESS_POINT_HXX
#define ACCESS_POINT_HXX

#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPointFactory.hxx>

#include <string>
#include <string_view>

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
     * @brief Construct a new AccessPoint object with the given network
     * interface name.
     *
     * @param interface The network interface name representing the access point.
     */
    AccessPoint(std::string_view interface);

    /**
     * @brief Get the network interface name representing the access point.
     *
     * @return std::string_view
     */
    std::string_view
    GetInterface() const noexcept override;

    /**
     * @brief Create a controller object.
     *
     * @return std::unique_ptr<Microsoft::Net::Wifi::IAccessPointController>
     */
    virtual std::unique_ptr<Microsoft::Net::Wifi::IAccessPointController>
    CreateController() override;

private:
    const std::string m_interface;
};

/**
 * @brief Creates instances of the AccessPoint class.
 */
struct AccessPointFactory :
    public IAccessPointFactory
{
    /**
     * @brief Create a new access point object for the given network interface.
     *
     * @param interface
     * @return std::shared_ptr<AccessPoint>
     */
    virtual std::shared_ptr<IAccessPoint>
    Create(std::string_view interface) override;
};
} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_HXX
