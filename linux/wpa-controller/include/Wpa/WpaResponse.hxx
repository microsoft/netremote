
#ifndef WPA_RESPONSE_HXX
#define WPA_RESPONSE_HXX

#include <string_view>
#include <string>

namespace Wpa
{
/**
 * @brief Object to hold generic, unparsed data from a wpa_supplicant or hostapd
 * command response.
 */
struct WpaResponse
{
    virtual ~WpaResponse() = default;

    WpaResponse() = default;
    WpaResponse(std::string_view payload);

    std::string Payload;
};
} // namespace Wpa

#endif // WPA_RESPONSE_HXX
