
#ifndef ACCESS_POINT_CONTROLLER_LINUX_HXX
#define ACCESS_POINT_CONTROLLER_LINUX_HXX

#include <string_view>

#include <Wpa/Hostapd.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <microsoft/net/wifi/AccessPointController.hxx>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Implementation of IAccessPointController which uses the hostapd daemon to control the access point.
 */
struct AccessPointControllerLinux :
    public AccessPointController
{
    virtual ~AccessPointControllerLinux() = default;

    AccessPointControllerLinux() = delete;

    /**
     * @brief Construct a new AccessPointControllerLinux object for the specified interface.
     *
     * @param interfaceName The name of the interface to control.
     */
    AccessPointControllerLinux(std::string_view interfaceName);

    /**
     * @brief Get whether the access point is enabled.
     *
     * @return true
     * @return false
     */
    virtual bool
    GetIsEnabled() override;

    /**
     * @brief Get the Capabilities object
     *
     * @return Ieee80211AccessPointCapabilities
     */
    virtual Ieee80211AccessPointCapabilities
    GetCapabilities() override;

    /**
     * @brief Set the Ieee80211 protocol.
     *
     * @param ieeeProtocol The Ieee80211 protocol to be set.
     * @return true
     * @return false
     */
    virtual bool
    SetProtocol(Microsoft::Net::Wifi::Ieee80211Protocol ieeeProtocol) override;

    /**
     * @brief Set the AKM suites used by the access point.
     *
     * @param akmSuites The AKM suites to be set.
     * @return true
     * @return false
     */
    virtual bool
    SetAkmSuites(std::vector<Microsoft::Net::Wifi::Ieee80211AkmSuite> akmSuites) override;

    /**
     * @brief Set the cipher suites used by the access point.
     *
     * @param cipherSuites The cipher suites to be set.
     * @return true
     * @return false
     */
    virtual bool
    SetCipherSuites(std::vector<Microsoft::Net::Wifi::Ieee80211CipherSuite> cipherSuites) override;

private:
    Wpa::Hostapd m_hostapd;
};

/**
 * @brief Factory to create AccessPointControllerLinux objects.
 */
struct AccessPointControllerLinuxFactory :
    public IAccessPointControllerFactory
{
    virtual ~AccessPointControllerLinuxFactory() = default;

    /**
     * @brief Create a new IAccessPointController object.
     *
     * @return std::unique_ptr<IAccessPointController>
     */
    virtual std::unique_ptr<IAccessPointController>
    Create(std::string_view interfaceName) override;
};
} // namespace Microsoft::Net::Wifi

#endif // ACCESS_POINT_CONTROLLER_LINUX_HXX
