
#include <array>
#include <chrono>
#include <cstdint>
#include <format>
#include <memory>
#include <mutex>

#include <Wpa/ProtocolWpa.hxx>
#include <Wpa/WpaEventHandler.hxx>
#include <notstd/Scope.hxx>
#include <plog/Log.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/types.h>
#include <unistd.h>
#include <wpa_ctrl.h>

using namespace Wpa;

WpaEventHandler::WpaEventHandler(std::unique_ptr<WpaControlSocketConnection> wpaControlSocketConnection, WpaType wpaType) :
    m_wpaControlSocketConnection(std::move(wpaControlSocketConnection)),
    m_wpaType(wpaType)
{
}

WpaEventHandler::~WpaEventHandler()
{
    StopListening();
}

WpaEventListenerRegistrationToken
WpaEventHandler::RegisterEventListener(std::weak_ptr<IWpaEventListener> wpaEventListener)
{
    std::unique_lock eventListenerWriteLock{ m_eventListenerStateGate };

    auto wpaEventListenerRegistrationToken = m_eventListenerRegistrationTokenNext++;
    m_eventListeners[wpaEventListenerRegistrationToken] = wpaEventListener;
    LOGD << std::format("Registered WPA event listener with eventListenerRegistrationToken '{}' on interface '{}'", wpaEventListenerRegistrationToken, m_wpaControlSocketConnection->GetInterfaceName());

    return wpaEventListenerRegistrationToken;
}

void
WpaEventHandler::UnregisterEventListener(WpaEventListenerRegistrationToken wpaEventListenerRegistrationToken)
{
    std::unique_lock eventListenerWriteLock{ m_eventListenerStateGate };

    const auto numRemoved = m_eventListeners.erase(wpaEventListenerRegistrationToken);
    if (numRemoved == 0) {
        LOGW << std::format("Attempted to unregister a WPA event listener that was not registered for interface '{}'.", m_wpaControlSocketConnection->GetInterfaceName());
    } else {
        LOGD << std::format("Unregistered WPA event listener with eventListenerRegistrationToken '{}' on interface '{}'", wpaEventListenerRegistrationToken, m_wpaControlSocketConnection->GetInterfaceName());
    }
}

void
WpaEventHandler::StartListening()
{
    std::unique_lock eventListenerWriteLock{ m_eventListenerStateGate };

    m_eventListenerThread = std::jthread([this](std::stop_token stopToken) mutable {
        ProcessEvents(*m_wpaControlSocketConnection, std::move(stopToken));
    });
}

void
WpaEventHandler::StopListening()
{
    std::shared_lock eventListenerReadLock{ m_eventListenerStateGate };

    // Check if the eventfd file descriptor for signaling thread stop has been created. If so, signal the thread to stop.
    if (m_fdEventFdStop != -1) {
        uint64_t value{ EventFdStopRequestValue };
        ssize_t numWritten = write(m_fdEventFdStop, &value, sizeof value);
        if (numWritten != sizeof value) {
            LOGE << std::format("Failed to write eventfd stop signal value for interface '{}'.", m_wpaControlSocketConnection->GetInterfaceName());
        }
    }

    if (m_eventListenerThread.joinable()) {
        m_eventListenerThread.request_stop();
        m_eventListenerThread.join();
        m_eventListenerThread = {};
    }
}

void
WpaEventHandler::ProcessNextEvent(WpaControlSocketConnection& wpaControlSocketConnection)
{
    const auto InterfaceName{ wpaControlSocketConnection.GetInterfaceName() };
    LOGD << std::format("Processing pending WPA event on interface '{}'.", InterfaceName);

    // Allocate a buffer for the event data, and subtract one to leave room for the null terminator.
    std::array<char, ProtocolWpa::EventSizeMax + 1> wpaEventBuffer{};
    std::size_t wpaEventBufferSize = std::size(wpaEventBuffer) - 1;

    // Retrieve the next WPA event from the control socket.
    int ret = wpa_ctrl_recv(*m_wpaControlSocketConnection, std::data(wpaEventBuffer), &wpaEventBufferSize);
    if (ret < 0) {
        LOGE << std::format("Failed to receive WPA event on interface '{}'.", InterfaceName);
        return;
    }

    // Explicitly null-terminate the buffer in case bad data was provided.
    wpaEventBuffer[wpaEventBufferSize] = '\0';

    // Record the time this event was received.
    const auto timestampNow{ std::chrono::system_clock::now() };
    std::string wpaEventPayload{ std::data(wpaEventBuffer), wpaEventBufferSize };

    // Create a WPA event args object to pass to the listeners.
    WpaEventArgs wpaEventArgs{
        .Timestamp = timestampNow,
        .Event = {
            .Source = m_wpaType,
            .Payload = { std::data(wpaEventBuffer), wpaEventBufferSize },
        },
    };

    // Make a copy of the event listeners to minimuze the time we hold the state lock. This is safe since the event
    // listeners are weak_ptrs which we later attempt to promote to a full shared_ptr before de-referencing them.
    decltype(m_eventListeners) eventListeners{};
    std::vector<decltype(m_eventListeners)::key_type> eventListenerRegistrationTokensExpired{};
    {
        std::shared_lock eventListenerReadLock{ m_eventListenerStateGate };
        eventListeners = m_eventListeners;
    }

    // Invoke each registered event listener with the event args.
    for (const auto& [eventListenerRegistrationToken, eventListenerWeak] : eventListeners) {
        std::shared_ptr<IWpaEventListener> eventListener = eventListenerWeak.lock();
        if (!eventListener) {
            LOGW << std::format("WPA event listener with registration token '{}' on interface '{}' expired; removing it", eventListenerRegistrationToken, InterfaceName);
            eventListenerRegistrationTokensExpired.push_back(eventListenerRegistrationToken);
            continue;
        }

        // Fire the event on the listener.
        eventListener->OnWpaEvent(this, &wpaEventArgs);
    }

    // Remove any expired listeners.
    if (!std::empty(eventListenerRegistrationTokensExpired)) {
        std::unique_lock eventListenerWriteLock{ m_eventListenerStateGate };
        for (const auto& eventListenerRegistrationToken : eventListenerRegistrationTokensExpired) {
            m_eventListeners.erase(eventListenerRegistrationToken);
        }
    }
}

void
WpaEventHandler::ProcessEvents(WpaControlSocketConnection& wpaControlSocketConnection, std::stop_token stopToken)
{
    const auto interfaceName{ wpaControlSocketConnection.GetInterfaceName() };
    LOGD << std::format("WPA event listener thread for interface '{}' started", interfaceName);
    auto logOnExit = notstd::ScopeExit([&] {
        LOGD << std::format("WPA event listener thread for interface '{}' stopped", interfaceName);
    });

    // One event for eventfd stop signaling, and one for WPA control socket event signaling.
    static constexpr int EpollEventsMax{ 2 };

    const auto InterfaceName{ wpaControlSocketConnection.GetInterfaceName() };

    std::array<epoll_event, EpollEventsMax> epollEvents = {};
    auto* eventEventFd = &epollEvents[0];
    auto* eventWpa = &epollEvents[1];

    // Create an epoll instance to listen for events on the eventfd file descriptor and WPA control socket.
    int ret{ -1 };
    int fdEpoll = epoll_create1(0);
    if (fdEpoll < 0) {
        ret = errno;
        LOGE << std::format("Failed to create epoll instance for interface '{}': {}", InterfaceName, ret);
        return;
    }

    auto closeEpollFileDescriptorOnExit = notstd::ScopeExit([&] {
        close(fdEpoll);
    });

    // Configure eventfd descriptor for thread stop signaling via epoll.
    int fdEventFdStop = eventfd(0, 0);
    if (fdEventFdStop < 0) {
        ret = errno;
        LOGE << std::format("Failed to create eventfd for interface '{}': {}", InterfaceName, ret);
        return;
    }

    m_fdEventFdStop = fdEventFdStop;

    auto closeEventFdStopFileDescriptorOnExit = notstd::ScopeExit([this] {
        close(m_fdEventFdStop);
        m_fdEventFdStop = -1;
    });

    eventEventFd->events = EPOLLIN;
    eventEventFd->data.fd = fdEventFdStop;

    // Add the eventfd descriptor to the epoll instance.
    ret = epoll_ctl(fdEpoll, EPOLL_CTL_ADD, fdEventFdStop, eventEventFd);
    if (ret < 0) {
        ret = errno;
        LOGE << std::format("Failed to add eventfd to epoll instance for interface '{}': {}", InterfaceName, ret);
        return;
    }

    struct wpa_ctrl* wpaControlSocket = wpaControlSocketConnection;

    // Configure WPA control socket for event signaling via epoll.
    int fdWpa = wpa_ctrl_get_fd(wpaControlSocket);
    if (fdWpa < 0) {
        ret = errno;
        LOGE << std::format("Failed to get WPA control socket file descriptor for interface '{}': {}", InterfaceName, ret);
        return;
    }

    eventWpa->events = EPOLLIN;
    eventWpa->data.fd = fdWpa;

    ret = epoll_ctl(fdEpoll, EPOLL_CTL_ADD, fdWpa, eventWpa);
    if (ret < 0) {
        ret = errno;
        LOGE << std::format("Failed to add WPA control socket to epoll instance for interface '{}': {}", InterfaceName, ret);
        return;
    }

    // Attach to WPA event stream.
    ret = wpa_ctrl_attach(wpaControlSocket);
    if (ret < 0) {
        ret = errno;
        LOGE << std::format("Failed to attach to WPA control socket for interface '{}': {}", InterfaceName, ret);
        return;
    }

    auto detachFromWpaControlSocketOnExit = notstd::ScopeExit([&] {
        wpa_ctrl_detach(wpaControlSocket);
    });

    bool stopRequested{ false };
    for (;;) {
        if (stopToken.stop_requested() || stopRequested) {
            LOGD << std::format("Stopping WPA event listener for interface '{}'.", InterfaceName);
            break;
        }

        // Wait for at least one event file descriptor to be signaled.
        const auto numEvents = epoll_wait(fdEpoll, std::data(epollEvents), std::size(epollEvents), -1);
        if (numEvents < 0) {
            ret = errno;
            LOGE << std::format("Failed to wait for events on interface '{}': {}", InterfaceName, ret);
            continue;
        }
        if (numEvents == 0) {
            LOGW << std::format("No events received on interface '{}'.", InterfaceName);
            continue;
        }

        // Determine which file descriptor was signaled, and handle it.
        for (std::size_t i = 0; i < std::size(epollEvents); ++i) {
            if (epollEvents[i].data.fd == fdEventFdStop) {
                LOGD << "Eventfd was signaled; checking for pending event(s).";
                uint64_t value{ ~EventFdStopRequestValue };
                ssize_t numRead = read(fdEventFdStop, &value, sizeof value);
                if (numRead != sizeof value) {
                    ret = errno;
                    LOGE << std::format("Failed to read from eventfd for interface '{}': numRead {} ({} {})", InterfaceName, numRead, ret, strerror(ret));
                    continue;
                }
                if (value == EventFdStopRequestValue) {
                    LOGD << std::format("Received stop signal on interface '{}'.", InterfaceName);
                    stopRequested = true;
                    break;
                }
            }
            if (epollEvents[i].data.fd == fdWpa) {
                LOGD << "WPA control socket was signaled; checking for pending event(s).";
                while (wpa_ctrl_pending(wpaControlSocket) > 0) {
                    ProcessNextEvent(wpaControlSocketConnection);
                }
            }
        }
    }
}
