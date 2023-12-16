
#include <Wpa/WpaCore.hxx>

std::string_view
Wpa::GetWpaTypeDaemonBinaryName(WpaType type) noexcept
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
