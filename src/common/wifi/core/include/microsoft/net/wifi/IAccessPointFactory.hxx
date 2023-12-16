
#ifndef I_ACCESS_POINT_FACTORY_HXX
#define I_ACCESS_POINT_FACTORY_HXX

#include <memory>
#include <string_view>

#include <microsoft/net/wifi/IAccessPoint.hxx>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Interface for creating access points.
 */
struct IAccessPointFactory
{
    /**
     * @brief Destroy the IAccessPointFactory object.
     */
    virtual ~IAccessPointFactory() = default;

    /**
     * @brief Create a new access point object for the given network interface.
     *
     * @param interface
     * @return std::shared_ptr<AccessPoint>
     */
    virtual std::shared_ptr<IAccessPoint>
    Create(std::string_view interface) = 0;
};
} // namespace Microsoft::Net::Wifi

#endif // I_ACCESS_POINT_FACTORY_HXX
