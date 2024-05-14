
#ifndef PROTOCOL_WPA_CONFIG_HXX
#define PROTOCOL_WPA_CONFIG_HXX

#ifndef CONFIG_WPA_CONTROL_SOCKET_PATH_BASE
#error "CONFIG_WPA_CONTROL_SOCKET_PATH_BASE must be defined."
#endif

namespace Wpa
{
/**
 * @brief Generic WPA configuration values that depend on build settings.
 *
 * Note that this is a generated file.
 */
struct ProtocolWpaConfig
{
    /**
     * @brief The common base (parent) path used for all WPA control sockets.
     */
    static constexpr auto ControlSocketPathBase{ CONFIG_WPA_CONTROL_SOCKET_PATH_BASE };

    /**
     * @brief The path to the control sockets used by wpa_supplicant.
     */
    static constexpr auto ControlSocketPathWpaSupplicant{ CONFIG_WPA_CONTROL_SOCKET_PATH_BASE "/wpa_supplicant" };

    /**
     * @brief The path to the control sockets used by hostapd.
     */
    static constexpr auto ControlSocketPathHostapd{ CONFIG_WPA_CONTROL_SOCKET_PATH_BASE "/hostapd" };
};
} // namespace Wpa

#undef CONFIG_WPA_CONTROL_SOCKET_PATH_BASE

#endif // PROTOCOL_WPA_CONFIG_HXX
