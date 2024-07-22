
#ifndef WPA_EVENT_ARGS_HXX
#define WPA_EVENT_ARGS_HXX

#include <chrono>

#include <Wpa/WpaEvent.hxx>

namespace Wpa
{
/**
 * @brief The event arguments for the WPA event.
 */
struct WpaEventArgs
{
    std::chrono::time_point<std::chrono::system_clock> Timestamp;
    WpaEvent Event;
};
} // namespace Wpa

#endif // WPA_EVENT_ARGS_HXX
