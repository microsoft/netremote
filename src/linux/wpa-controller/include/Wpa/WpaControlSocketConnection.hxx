
#ifndef WPA_CONTROL_SOCKET_CONNECTION_HXX
#define WPA_CONTROL_SOCKET_CONNECTION_HXX

#include <filesystem>
#include <memory>
#include <string_view>

#include <wpa_ctrl.h>

namespace Wpa
{
/**
 * @brief Helper class to create a connection on a WPA control socket.
 *
 * This automatically manages the resources required to connect to a WPA control socket.
 */
class WpaControlSocketConnection
{
public:
    ~WpaControlSocketConnection();

    operator struct wpa_ctrl *() const noexcept; // NOLINT(hicpp-explicit-conversions)

    /**
     * @brief Attempt to make a connection to a WPA control socket for the specicied interface.
     *
     * @param controlSocketPathDir The directory containing the control socket.
     * @param interfaceName The name of the interface to connect to.
     * @return std::unique_ptr<WpaControlSocketConnection>
     */
    static std::unique_ptr<WpaControlSocketConnection>
    TryCreate(std::filesystem::path controlSocketPathDir, std::string_view interfaceName);

    /**
     * @brief Attempt to make a connection to a WPA control socket for the specicied interface.
     *
     * @param forceReconnect Force a reconnection to the control socket if it is already connected.
     * @return true The connection was successful.
     * @return false The connection was not successful.
     */
    bool
    Connect(bool forceReconnect = false) noexcept;

    /**
     * @brief Disconnect from the control socket.
     */
    void
    Disconnect() noexcept;

protected:
    /**
     * @brief Construct a new WpaControlSocketConnection object.
     *
     * @param controlSocketPathDir The directory containing the control socket.
     * @param interfaceName The name of the interface to connect to.
     */
    WpaControlSocketConnection(std::filesystem::path controlSocketPathDir, std::string_view interfaceName);

private:
    std::filesystem::path m_controlSocketPath;
    struct wpa_ctrl* m_controlSocket{ nullptr };
};
} // namespace Wpa

#endif // WPA_CONTROL_SOCKET_CONNECTION_HXX
