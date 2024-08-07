
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
#include <Wpa/WpaControlSocketConnection.hxx>
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
    virtual ~WpaController() = default;

    /**
     * @brief Determines if this controller is valid, meaning that it can be used to control the interface for which it
     * was configured. TThe controller will be invalid if the control socket path is not valid, either because it is
     * inacessible (bad permissions) or it does not exist (wpa_supplicant or hostapd is not controlling this interface).
     * 
     * @return true 
     * @return false 
     */
    bool
    IsValid() const noexcept;

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

private:
    /**
     * @brief Get a control socket connection. If one does not exist, one will be established and cached for future use.
     *
     * @return WpaControlSocketConnection*
     */
    WpaControlSocketConnection*
    GetCommandControlSocketConnection();

private:
    const WpaType m_type;
    const std::string m_interfaceName;
    std::filesystem::path m_controlSocketPath;
    // Protects m_controlSocketCommandConnection.
    std::shared_mutex m_controlSocketCommandConnectionGate;
    std::unique_ptr<WpaControlSocketConnection> m_controlSocketCommandConnection;
};
} // namespace Wpa

#endif // WPA_CONTROLLER_HXX
