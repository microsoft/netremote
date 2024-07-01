
#ifndef WPA_RESPONSE_GET_CONFIG_HXX
#define WPA_RESPONSE_GET_CONFIG_HXX

#include <Wpa/ProtocolHostapd.hxx>
#include <Wpa/WpaResponse.hxx>

namespace Wpa
{
/**
 * @brief Representation of the response to the "GET_CONFIG" command.
 */
struct WpaResponseGetConfig :
    public WpaResponse
{
    WpaResponseGetConfig() = default;

    HostapdBssConfiguration Configuration;
};
} // namespace Wpa

#endif // WPA_RESPONSE_COMMAND_GET_CONFIG_HXX
