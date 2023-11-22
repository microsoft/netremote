
#ifndef WPA_COMMAND_HXX
#define WPA_COMMAND_HXX

#include <string_view>
#include <string>

namespace Wpa
{
/**
 * @brief Object to hold generic command data for a wpa_supplicant or hostapd
 * request.
 */
struct WpaCommand
{
    constexpr WpaCommand() = default;
    constexpr WpaCommand(std::string_view data) :
        Data(data)
    {
    }

    std::string Data;
};
} // namespace Wpa

#endif // WPA_COMMAND_HXX
