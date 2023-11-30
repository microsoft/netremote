
#ifndef ACCESS_POINT_HXX
#define ACCESS_POINT_HXX

#include <string_view>
#include <string>

namespace Microsoft::Net::Wifi
{
/**
 * @brief The AccessPoint struct represents a wireless access point.
 */
struct AccessPoint
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
    std::string_view GetInterface() const noexcept;

private:
    const std::string m_interface;
};
} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_HXX
