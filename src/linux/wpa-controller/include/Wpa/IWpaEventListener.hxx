
#ifndef I_WPA_EVENT_HANDLER_HXX
#define I_WPA_EVENT_HANDLER_HXX

#include <Wpa/WpaEventArgs.hxx>

namespace Wpa
{
/**
 * @brief Represents the sender or source of a WPA event.
 */
struct WpaEventSender
{
    virtual ~WpaEventSender() = default;
};

/**
 * @brief Interface for WPA event consumers.
 */
struct IWpaEventListener
{
    virtual ~IWpaEventListener() = default;

    /**
     * @brief Invoked when a WPA event is received.
     *
     * @param sender The sender or source of the event.
     * @param eventArgs The event arguments.
     */
    virtual void
    OnWpaEvent(WpaEventSender *sender, const WpaEventArgs *eventArgs) = 0;
};
} // namespace Wpa

#endif // I_WPA_EVENT_HANDLER_HXX
