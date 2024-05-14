
#ifndef WPA_CORE_HXX
#define WPA_CORE_HXX

#include <string_view>

namespace Wpa
{
/**
 * @brief The type of WPA daemon/service.
 */
enum class WpaType {
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
    default:
        return "unknown";
    }
}

} // namespace Wpa

#endif // WPA_CORE_HXX
