
#ifndef ACCESS_POINT_HXX
#define ACCESS_POINT_HXX

#include <microsoft/net/wifi/IAccessPoint.hxx>

#include <string_view>
#include <string>

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
    std::string_view GetInterface() const noexcept override;

private:
    const std::string m_interface;
};
} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_HXX
