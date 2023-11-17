
#ifndef WPA_CONTROLLER_HXX
#define WPA_CONTROLLER_HXX

#include <filesystem>
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
     * @brief Construct a new WpaController object for the specified interface
     * with a default control socket path.
     * 
     * @param interfaceName The name of the interface to control. Eg. wlan1.
     * @param type The type of daemon controlling the interface.
     */
    WpaController(std::string_view interfaceName, WpaType type);

    /**
     * @brief Construct a new WpaController object for the specified interface
     * using the specified control socket.
     * 
     * @param interfaceName The name of the interface to control. Eg. wlan1.
     * @param type The type of daemon controlling the interface.
     * @param controlSocketPath The full path to the daemon control socket.
     */
    WpaController(std::string_view interfaceName, WpaType type, std::string_view controlSocketPath);

    /**
     * @brief The type of daemon this object is controlling.
     * 
     * @return WpaType 
     */
    WpaType Type() const noexcept;

    /**
     * @brief The path of the control socket this object is using.
     * 
     * @return std::filesystem::path 
     */
    std::filesystem::path ControlSocketPath() const noexcept;

    /**
     * @brief Helper class for working with the wpa control socket.
     */
    struct WpaControlSocket
    {
        WpaControlSocket() = delete;

        static constexpr auto DefaultPathHostapd = "/var/run/hostapd";
        static constexpr auto DefaultPathWpaSupplicant = "/var/run/wpa_supplicant";

        /**
         * @brief Get the default path for the control socket of the specified daemon.
         * 
         * @param wpaType The wpa daemon type to get the default control socket for.
         * @return constexpr auto A string containing the default control socket path.
         */
        static constexpr auto DefaultPath(WpaType wpaType)
        {
            switch (wpaType)
            {
            case WpaType::Hostapd:
                return DefaultPathHostapd;
            case WpaType::WpaSupplicant:
                return DefaultPathWpaSupplicant;
            default:
                throw std::runtime_error("Unknown WpaType");
            }
        }
    };

private:
    const WpaType m_type;
    const std::string m_interfaceName;
    std::filesystem::path m_controlSocketPath;
};
} // namespace Wpa

#endif // WPA_CONTROLLER_HXX
