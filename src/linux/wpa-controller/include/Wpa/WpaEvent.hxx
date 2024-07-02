
#ifndef WPA_EVENT_HXX
#define WPA_EVENT_HXX

#include <string>

#include <Wpa/WpaCore.hxx>

namespace Wpa
{
/**
 * @brief Represents an event from a WPA daemon/service.
 */
struct WpaEvent
{
    WpaType Source;
    std::string Payload;
};

} // namespace Wpa

#endif // WPA_EVENT_HXX
