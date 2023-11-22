
#ifndef WPA_RESPONSE_STATUS_HXX
#define WPA_RESPONSE_STATUS_HXX

#include <Wpa/WpaResponse.hxx>
#include <Wpa/ProtocolHostapd.hxx>

namespace Wpa
{
struct WpaResponseStatus
    : public WpaResponse
{
    WpaResponseStatus() = default;

    HostapdStatus Status;
};
} // namespace Wpa

#endif // WPA_RESPONSE_COMMAND_STATUS_HXX
