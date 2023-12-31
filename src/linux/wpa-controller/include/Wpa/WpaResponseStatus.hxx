
#ifndef WPA_RESPONSE_STATUS_HXX
#define WPA_RESPONSE_STATUS_HXX

#include <Wpa/ProtocolHostapd.hxx>
#include <Wpa/WpaResponse.hxx>

namespace Wpa
{
/**
 * @brief Representation of the response to the "STATUS" command.
 */
struct WpaResponseStatus :
    public WpaResponse
{
    WpaResponseStatus() = default;

    HostapdStatus Status;
};
} // namespace Wpa

#endif // WPA_RESPONSE_COMMAND_STATUS_HXX
