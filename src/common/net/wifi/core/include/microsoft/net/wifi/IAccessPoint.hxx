
#ifndef I_ACCESS_POINT_HXX
#define I_ACCESS_POINT_HXX

#include <memory>
#include <string_view>

#include <microsoft/net/wifi/IAccessPointController.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>

namespace Microsoft::Net::Wifi
{

/**
 * @brief Container to hold static properties about an access point.
 */
struct AccessPointProperties
{
    std::vector<std::string> Properties{};
};

/**
 * @brief Represents a wireless access point.
 */
struct IAccessPoint
{
    IAccessPoint() = default;

    /**
     * @brief Destroy the IAccessPoint object.
     */
    virtual ~IAccessPoint() = default;

    /**
     * Prevent copying and moving of IAccessPoint objects.
     */
    IAccessPoint(const IAccessPoint&) = delete;

    IAccessPoint(IAccessPoint&&) = delete;

    IAccessPoint&
    operator=(const IAccessPoint&) = delete;

    IAccessPoint&
    operator=(IAccessPoint&&) = delete;

    /**
     * @brief Get the network interface name representing the access point.
     *
     * @return std::string_view
     */
    virtual std::string_view
    GetInterfaceName() const noexcept = 0;

    /**
     * @brief Get the mac address of the access point.
     *
     * @return Ieee80211MacAddress
     */
    virtual Ieee80211MacAddress
    GetMacAddress() const noexcept = 0;

    /**
     * @brief Get the static properties of an access point.
     *
     * @return AccessPointProperties
     */
    virtual AccessPointProperties
    GetProperties() const noexcept = 0;

    /**
     * @brief Create a new instance that can control the access point.
     *
     * @return std::unique_ptr<IAccessPointController>
     */
    virtual std::unique_ptr<IAccessPointController>
    CreateController() = 0;
};

/**
 * @brief Arguments to be passed to the Create method of IAccessPointFactory.
 */
struct IAccessPointCreateArgs
{
    IAccessPointCreateArgs() = default;

    virtual ~IAccessPointCreateArgs() = default;

    /**
     * Prevent copying and moving of IAccessPointCreateArgs objects.
     */
    IAccessPointCreateArgs(const IAccessPointCreateArgs&) = delete;

    IAccessPointCreateArgs(IAccessPointCreateArgs&&) = delete;

    IAccessPointCreateArgs&
    operator=(const IAccessPointCreateArgs&) = delete;

    IAccessPointCreateArgs&
    operator=(IAccessPointCreateArgs&&) = delete;
};

/**
 * @brief Interface for creating access points.
 */
struct IAccessPointFactory
{
    IAccessPointFactory() = default;

    /**
     * @brief Destroy the IAccessPointFactory object.
     */
    virtual ~IAccessPointFactory() = default;

    /**
     * Prevent copying and moving of IAccessPointFactory objects.
     */
    IAccessPointFactory(const IAccessPointFactory&) = delete;

    IAccessPointFactory(IAccessPointFactory&&) = delete;

    IAccessPointFactory&
    operator=(const IAccessPointFactory&) = delete;

    IAccessPointFactory&
    operator=(IAccessPointFactory&&) = delete;

    /**
     * @brief Create a new access point object for the given network interface.
     *
     * @param interfaceName The name of the interface.
     * @return std::shared_ptr<IAccessPoint>
     */
    virtual std::shared_ptr<IAccessPoint>
    Create(std::string_view interfaceName) = 0;

    /**
     * @brief Create a new access point object for the given network interface with the specified creation arguments.
     *
     * @param interfaceName The name of the interface.
     * @param createArgs Arguments to be passed to the access point during creation.
     * @return std::shared_ptr<IAccessPoint>
     */
    virtual std::shared_ptr<IAccessPoint>
    Create(std::string_view interfaceName, std::unique_ptr<IAccessPointCreateArgs> createArgs) = 0;
};
} // namespace Microsoft::Net::Wifi

#endif // I_ACCESS_POINT_HXX
