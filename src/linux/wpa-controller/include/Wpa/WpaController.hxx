
#ifndef WPA_CONTROLLER_HXX
#define WPA_CONTROLLER_HXX

#include <filesystem>
#include <memory>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <string_view>

#include <Wpa/ProtocolWpaConfig.hxx>
#include <Wpa/WpaCommand.hxx>
#include <Wpa/WpaCore.hxx>
#include <Wpa/WpaResponse.hxx>

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
     * @brief Construct a new WpaController object for the specified interface
     * using the specified control socket.
     *
     * @param interfaceName The name of the interface to control. Eg. wlan1.
     * @param type The type of daemon controlling the interface.
     * @param controlSocketPath The full path to the daemon control socket.
     */
    WpaController(std::string_view interfaceName, WpaType type, std::filesystem::path controlSocketPath);

    WpaController(const WpaController&) = delete;

    WpaController(WpaController&&) = delete;

    WpaController&
    operator=(const WpaController&) = delete;

    WpaController&
    operator=(WpaController&&) = delete;

    /**
     * @brief Destroy the WpaController object.
     */
    virtual ~WpaController();

    /**
     * @brief The type of daemon this object is controlling.
     *
     * @return WpaType
     */
    WpaType
    Type() const noexcept;

    /**
     * @brief The path of the control socket this object is using.
     *
     * @return std::filesystem::path
     */
    std::filesystem::path
    ControlSocketPath() const noexcept;

    /**
     * @brief Send a command over the control socket and return the response
     * synchronously.
     *
     * This will not receive any unsolicited event messages.
     *
     * @param command The command to send.
     * @return std::shared_ptr<WpaResponse>
     */
    std::shared_ptr<WpaResponse>
    SendCommand(const WpaCommand& command);

    /**
     * @brief Syntactic sugar for returning a specific derived response type.
     *
     * @tparam ResponseT The type of response to return.
     * @param command The command to send.
     * @return std::shared_ptr<ResponseT>
     */
    // clang-format off
    template <typename ResponseT>
    requires std::derived_from<ResponseT, WpaResponse>
    // clang-format on
    std::shared_ptr<ResponseT>
    SendCommand(const WpaCommand& command)
    {
        return std::dynamic_pointer_cast<ResponseT>(SendCommand(command));
    }

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
    };

private:
    /**
     * @brief Get the control socket object. If a socket connection does not
     * exist, one will be established.
     *
     * @return struct wpa_ctrl*
     */
    struct wpa_ctrl*
    GetCommandControlSocket();

private:
    const WpaType m_type;
    const std::string m_interfaceName;
    std::filesystem::path m_controlSocketPath;
    // Protects m_controlSocketCommand.
    std::shared_mutex m_controlSocketCommandGate;
    struct wpa_ctrl* m_controlSocketCommand{ nullptr };
};
} // namespace Wpa

#endif // WPA_CONTROLLER_HXX
