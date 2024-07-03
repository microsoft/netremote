
#ifndef WPA_CORE_HXX
#define WPA_CORE_HXX

#include <string_view>

namespace Wpa
{
/**
 * @brief Log levels for WPA messages and events.
 */
enum class WpaLogLevel : int32_t {
    Unknown = -1,
    Excessive = 0,
    MessageDump = 1,
    Debug = 2,
    Info = 3,
    Warning = 4,
    Error = 5,
};

/**
 * @brief The type of WPA daemon/service.
 */
enum class WpaType {
    Unknown,
    Hostapd,
    WpaSupplicant,
};

/**
 * @brief Get the WpaType associated daemon binary name.
 *
 * @param type The WpaType to obtain the daemon binary name for.
 * @return std::string_view
 */
static constexpr std::string_view
GetWpaTypeDaemonBinaryName(WpaType type) noexcept
{
    switch (type) {
    case WpaType::Hostapd:
        return "hostapd";
    case WpaType::WpaSupplicant:
        return "wpa_supplicant";
    case WpaType::Unknown:
        [[fallthrough]];
    default:
        return "unknown";
    }
}

} // namespace Wpa

#endif // WPA_CORE_HXX
