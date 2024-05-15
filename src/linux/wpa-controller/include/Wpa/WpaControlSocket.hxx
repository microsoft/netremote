
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
     * @brief Determine if the specified control socket path is valid for the specified interface.
     * 
     * @param controlSocketPath The control socket path to check.
     * @param interfaceName The interface name for the control socket.
     * @return true 
     * @return false 
     */
    static bool
    IsPathValidForInterface(std::filesystem::path controlSocketPath, std::string_view interfaceName);
};
} // namespace Wpa

#endif // WPA_CONTROL_SOCKET_HXX
