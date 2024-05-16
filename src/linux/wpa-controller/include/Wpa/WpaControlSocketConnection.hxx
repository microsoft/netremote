
#ifndef WPA_CONTROL_SOCKET_CONNECTION_HXX
#define WPA_CONTROL_SOCKET_CONNECTION_HXX

#include <filesystem>
#include <memory>
#include <string_view>

#include <wpa_ctrl.h>

namespace Wpa
{
/**
 * @brief Policy to use when an existing connection is detected.
 */
enum class ExistingConnectionPolicy {
    Reconnect,
    Ignore,
};

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
     * @param interfaceName The name of the interface to connect to.
     * @param controlSocketPathDir The directory containing the control socket.
     * @return std::unique_ptr<WpaControlSocketConnection>
     */
    static std::unique_ptr<WpaControlSocketConnection>
    TryCreate(std::string_view interfaceName, std::filesystem::path controlSocketPathDir);

    /**
     * @brief Attempt to make a connection to a WPA control socket for the specicied interface.
     *
     * @param existingConnectionPolicy The policy to use when an existing connection is detected.
     * @return true The connection was successful.
     * @return false The connection was not successful.
     */
    bool
    Connect(ExistingConnectionPolicy existingConnectionPolicy = ExistingConnectionPolicy::Ignore) noexcept;

    /**
     * @brief Disconnect from the control socket.
     */
    void
    Disconnect() noexcept;

protected:
    /**
     * @brief Construct a new WpaControlSocketConnection object.
     *
     * @param interfaceName The name of the interface to connect to.
     * @param controlSocketPathDir The directory containing the control socket.
     */
    WpaControlSocketConnection(std::string_view interfaceName, std::filesystem::path controlSocketPathDir);

private:
    std::filesystem::path m_controlSocketPath;
    struct wpa_ctrl* m_controlSocket{ nullptr };
};
} // namespace Wpa

#endif // WPA_CONTROL_SOCKET_CONNECTION_HXX
