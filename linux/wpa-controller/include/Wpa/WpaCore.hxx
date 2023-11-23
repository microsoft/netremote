
#ifndef WPA_CORE_HXX
#define WPA_CORE_HXX

#include <string_view>

namespace Wpa
{
/**
 * @brief The type of WPA daemon/service.
 */
enum class WpaType
{
    Hostapd,
    WpaSupplicant
};

/**
 * @brief Get the WpaType associated daemon binary name.
 * 
 * @param type The WpaType to obtain the daemon binary name for.
 * @return std::string_view 
 */
std::string_view GetWpaTypeDaemonBinaryName(WpaType type) noexcept;

} // namespace Wpa

#endif // WPA_CORE_HXX
