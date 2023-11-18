
#ifndef WPA_COMMAND_HXX
#define WPA_COMMAND_HXX

#include <string>

namespace Wpa
{
/**
 * @brief Object to hold generic command data for a wpa_supplicant or hostapd
 * request.
 */
struct WpaCommand
{
    std::string Data;
};
} // namespace Wpa

#endif // WPA_COMMAND_HXX
