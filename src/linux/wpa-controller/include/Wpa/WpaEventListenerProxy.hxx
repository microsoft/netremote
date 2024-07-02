
#ifndef WPA_EVENT_LISTENER_PROXY_HXX
#define WPA_EVENT_LISTENER_PROXY_HXX

#include <memory>

#include <Wpa/IWpaEventListener.hxx>

namespace Wpa
{
/**
 * @brief Helper to allow use of classes that implement IWpaEventListener without providing a std::weak_ptr<> of itself
 * to WpaEventHandler.
 *
 * The implemenation simply forwards the OnWpaEvent() call to the IWpaEventListener instance provided in the
 * constructor. The caller is responsible for ensuring that the IWpaEventListener instance outlives the
 * WpaEventListenerProxy instance.
 */
struct WpaEventListenerProxy :
    public IWpaEventListener,
    public std::enable_shared_from_this<WpaEventListenerProxy>
{
    virtual ~WpaEventListenerProxy() = default;

    /**
     * @brief Create a new WpaEventListenerProxy instance that forwards OnWpaEvent() calls to the provided
     * IWpaEventListener.
     *
     * @param wpaEventListenerProxy The IWpaEventListener instance to forward OnWpaEvent() calls to.
     * @return std::shared_ptr<WpaEventListenerProxy>
     */
    static std::shared_ptr<WpaEventListenerProxy>
    Create(IWpaEventListener &wpaEventListenerProxy);

    /**
     * @brief Invoked when a WPA event is received.
     *
     * @param sender The sender or source of the event.
     * @param eventArgs The event arguments.
     */
    void
    OnWpaEvent(WpaEventSender *sender, const WpaEventArgs *eventArgs) override;

protected:
    /**
     * @brief Construct a new WpaEventListenerProxy instance that forwards OnWpaEvent() calls to the provided
     * IWpaEventListener.
     *
     * @param wpaEventListenerProxy The IWpaEventListener instance to forward OnWpaEvent() calls to.
     */
    WpaEventListenerProxy(IWpaEventListener &wpaEventListenerProxy);

private:
    IWpaEventListener &m_wpaEventListenerProxy;
};
} // namespace Wpa

#endif // WPA_EVENT_LISTENER_PROXY_HXX
