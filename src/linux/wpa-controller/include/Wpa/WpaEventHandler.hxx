
#ifndef WPA_EVENT_HANDLER_HXX
#define WPA_EVENT_HANDLER_HXX

#include <cstdint>
#include <filesystem>
#include <memory>
#include <shared_mutex>
#include <stop_token>
#include <string_view>
#include <thread>
#include <unordered_map>

#include <Wpa/IWpaEventListener.hxx>
#include <Wpa/WpaControlSocketConnection.hxx>
#include <Wpa/WpaCore.hxx>

namespace Wpa
{
using WpaEventListenerRegistrationToken = uint32_t;

/**
 * @brief Class that processes WPA events and distributes them to registered listeners.
 */
struct WpaEventHandler :
    public WpaEventSender
{
    /**
     * @brief Create a new WpaEventHandler.
     *
     * @param wpaControlSocketConnection
     */
    WpaEventHandler(std::unique_ptr<WpaControlSocketConnection> wpaControlSocketConnection, WpaType wpaType);

    /**
     * @brief Destroy the WpaEventHandler object.
     */
    ~WpaEventHandler();

    /**
     * @brief Register a listener for WPA events.
     *
     * @param wpaEventListener The listener to register.
     * @return WpaEventListenerRegistrationToken A token that can be used to unregister the listener.
     */
    WpaEventListenerRegistrationToken
    RegisterEventListener(std::weak_ptr<IWpaEventListener> wpaEventListener);

    /**
     * @brief Unregister a listener for WPA events.
     *
     * @param wpaEventListenerRegistrationToken The token returned by RegisterEventListener.
     */
    void
    UnregisterEventListener(WpaEventListenerRegistrationToken wpaEventListenerRegistrationToken);

    /**
     * @brief Start listening for WPA events.
     */
    void
    StartListening();

    /**
     * @brief Stop listening for WPA events.
     */
    void
    StopListening();

private:
    /**
     * @brief Process the next WPA event.
     *
     * @param WpaControlSocketConnection The WPA control socket connection to use.
     */
    void
    ProcessNextEvent(WpaControlSocketConnection& wpaControlSocketConnection);

    /**
     * @brief Listen for and process WPA events.
     *
     * @param wpaControlSocketConnection The WPA control socket connection to listen on.
     * @param stopToken The stop token to use to stop listening.
     */
    void
    ProcessEvents(WpaControlSocketConnection& wpaControlSocketConnection, std::stop_token stopToken);

private:
    std::unique_ptr<WpaControlSocketConnection> m_wpaControlSocketConnection{ nullptr };
    WpaType m_wpaType;

    // The below m_eventListenerStateGate mutex protects m_eventListeners, m_eventListenerRegistrationTokenNext, and
    // m_eventListenerThread.
    std::shared_mutex m_eventListenerStateGate;
    std::unordered_map<WpaEventListenerRegistrationToken, std::weak_ptr<IWpaEventListener>> m_eventListeners;
    WpaEventListenerRegistrationToken m_eventListenerRegistrationTokenNext{ 0 };
    std::jthread m_eventListenerThread;

    // Signal value to stop the event listener thread.
    static constexpr std::uint64_t EventFdStopRequestValue{ 1 };
    int m_fdEventFdStop{ -1 };
};
} // namespace Wpa

#endif // WPA_EVENT_HANDLER_HXX
