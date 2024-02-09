
#ifndef I_ACCESS_POINT_CONTROLLER_HXX
#define I_ACCESS_POINT_CONTROLLER_HXX

#include <exception>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <microsoft/net/wifi/Ieee80211.hxx>
#include <microsoft/net/wifi/Ieee80211AccessPointCapabilities.hxx>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Top-level exception type that may ber thrown by IAccessPointController operations.
 */
struct AccessPointControllerException :
    public std::exception
{
    explicit AccessPointControllerException(std::string_view what);

    const char*
    what() const noexcept override;

private:
    std::string m_what;
};

/**
 * @brief Class allowing control of an access point.
 */
struct IAccessPointController
{
    IAccessPointController() = default;

    /**
     * @brief Destroy the IAccessPointController object.
     */
    virtual ~IAccessPointController() = default;

    /**
     * Prevent copying and moving of IAccessPointController objects. 
     */
    IAccessPointController(const IAccessPointController&) = delete;
    IAccessPointController& operator=(const IAccessPointController&) = delete;
    IAccessPointController(IAccessPointController&&) = delete;
    IAccessPointController& operator=(IAccessPointController&&) = delete;

    /**
     * @brief Get the interface name associated with this controller.
     *
     * @return std::string_view
     */
    virtual std::string_view
    GetInterfaceName() const = 0;

    /**
     * @brief Get whether the access point is enabled.
     *
     * @return true
     * @return false
     */
    virtual bool
    GetIsEnabled() = 0;

    /**
     * @brief Get the capabilities of the access point.
     *
     * @return Ieee80211AccessPointCapabilities
     */
    virtual Ieee80211AccessPointCapabilities
    GetCapabilities() = 0;

    /**
     * @brief Set the Ieee80211 protocol of the access point.
     *
     * @param ieeeProtocol The Ieee80211 protocol to be set.
     * @return true
     * @return false
     *
     */
    virtual bool
    SetProtocol(Microsoft::Net::Wifi::Ieee80211Protocol ieeeProtocol) = 0;

    /**
     * @brief Set the frquency bands the access point should enable.
     *
     * @param frequencyBands The frequency bands to be set.
     * @return true
     * @return false
     */
    virtual bool
    SetFrequencyBands(std::vector<Microsoft::Net::Wifi::Ieee80211FrequencyBand> frequencyBands) = 0;
};

/**
 * @brief Factory for creating IAccessPointController objects.
 */
struct IAccessPointControllerFactory
{
    IAccessPointControllerFactory() = default;

    virtual ~IAccessPointControllerFactory() = default;

    /**
     * Prevent copying and moving of IAccessPointControllerFactory objects. 
     */
    IAccessPointControllerFactory(const IAccessPointControllerFactory&) = delete;
    IAccessPointControllerFactory& operator=(const IAccessPointControllerFactory&) = delete;
    IAccessPointControllerFactory(IAccessPointControllerFactory&&) = delete;
    IAccessPointControllerFactory& operator=(IAccessPointControllerFactory&&) = delete;

    /**
     * @brief Create a new IAccessPointController object.
     *
     * @return std::unique_ptr<IAccessPointController>
     */
    virtual std::unique_ptr<IAccessPointController>
    Create(std::string_view interfaceName) = 0;
};
} // namespace Microsoft::Net::Wifi

#endif // I_ACCESS_POINT_CONTROLLER_HXX
