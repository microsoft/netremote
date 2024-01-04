
#ifndef I_ACCESS_POINT_HXX
#define I_ACCESS_POINT_HXX

#include <memory>
#include <string_view>

namespace Microsoft::Net::Wifi
{
struct IAccessPointController;

/**
 * @brief Represents a wireless access point.
 */
struct IAccessPoint
{
    /**
     * @brief Destroy the IAccessPoint object.
     */
    virtual ~IAccessPoint() = default;

    /**
     * @brief Get the network interface name representing the access point.
     *
     * @return std::string_view
     */
    virtual std::string_view
    GetInterface() const noexcept = 0;

    /**
     * @brief Create a new instance that can control the access point.
     *
     * @return std::unique_ptr<IAccessPointController>
     */
    virtual std::unique_ptr<IAccessPointController>
    CreateController() = 0;
};
} // namespace Microsoft::Net::Wifi

#endif // I_ACCESS_POINT_HXX
