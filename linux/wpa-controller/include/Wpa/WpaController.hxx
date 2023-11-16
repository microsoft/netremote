
#ifndef WPA_CONTROLLER_HXX
#define WPA_CONTROLLER_HXX

#include <string>
#include <string_view>

#include <Wpa/WpaCore.hxx>

namespace Wpa
{
/**
 * @brief Controls daemons from the hostap project, including wpa_supplicant and
 * hostapd.
 */
struct WpaController
{
    /**
     * @brief Construct a new WpaController object for the specified interface.
     * 
     * @param interfaceName The name of the interface to control. Eg. wlan1.
     * @param type The type of daemon controlling the interface.
     */
    WpaController(std::string_view interfaceName, WpaType type);

    /**
     * @brief The type of daemon this object is controlling.
     * 
     * @return WpaType 
     */
    WpaType Type() const noexcept;

private:
    const WpaType m_type;
    const std::string m_interfaceName;
};
} // namespace Wpa

#endif // WPA_CONTROLLER_HXX
