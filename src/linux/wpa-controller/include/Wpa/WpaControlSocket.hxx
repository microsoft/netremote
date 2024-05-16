
#ifndef WPA_CONTROL_SOCKET_HXX
#define WPA_CONTROL_SOCKET_HXX

#include <filesystem>
#include <stdexcept>
#include <string_view>

#include <Wpa/ProtocolWpaConfig.hxx>
#include <Wpa/WpaCore.hxx>

namespace Wpa
{
/**
 * @brief Helper class for working with the wpa control socket.
 */
struct WpaControlSocket
{
    WpaControlSocket() = delete;

    /**
     * @brief Maximum WPA control interface message size, im bytes. The
     * official wpa_cli tool uses this as an upper bound, so is used
     * similarly here.
     */
    static constexpr auto MessageSizeMax = 4096;

    /**
     * @brief Get the default path for the control socket of the specified daemon.
     *
     * @param wpaType The wpa daemon type to get the default control socket for.
     * @return constexpr auto A string containing the default control socket path.
     */
    static constexpr auto
    DefaultPath(WpaType wpaType)
    {
        switch (wpaType) {
        case WpaType::Hostapd:
            return ProtocolWpaConfig::ControlSocketPathHostapd;
        case WpaType::WpaSupplicant:
            return ProtocolWpaConfig::ControlSocketPathWpaSupplicant;
        default:
            throw std::runtime_error("Unknown WpaType");
        }
    }

    /**
     * @brief Determine if a control socket exists for the specified interface and control socket path.
     *
     * @param interfaceName The interface to check.
     * @param wpaType The type of wpa daemon to check for.
     * @return true If a control socket exists for the specified interface.
     * @return false If a control socket does not exist for the specified interface.
     */
    static bool
    Exists(std::string_view interfaceName, WpaType wpaType);

    /**
     * @brief Determine if a control socket exists for the specified interface and control socket path.
     *
     * @param interfaceName The interface to check.
     * @param controlSocketPathDir The directory containing the control socket.
     * @return true If a control socket exists for the specified interface.
     * @return false If a control socket does not exist for the specified interface.
     */
    static bool
    Exists(std::string_view interfaceName, std::filesystem::path controlSocketPathDir);
};
} // namespace Wpa

#endif // WPA_CONTROL_SOCKET_HXX
