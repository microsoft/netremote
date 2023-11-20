
#ifndef WPA_DAEMON_CATCH2_EVENT_LISTENER_HXX
#define WPA_DAEMON_CATCH2_EVENT_LISTENER_HXX

#include <memory>
#include <unordered_map>

#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>
#include <Wpa/WpaCore.hxx>

#include "WpaDaemonInstance.hxx"

struct WpaDaemonCatch2EventListener : public Catch::EventListenerBase
{
    // Inherit base class constructor.
    using EventListenerBase::EventListenerBase; 

    void testCaseStarting(Catch::TestCaseInfo const& testInfo) override;

    void testCaseEnded(Catch::TestCaseStats const& testCaseStats) override;

private:
    std::unordered_map<Wpa::WpaType, std::unique_ptr<IWpaDaemonInstance>> m_wpaDaemonInstances;
};

CATCH_REGISTER_LISTENER(WpaDaemonCatch2EventListener)

#endif // WPA_DAEMON_CATCH2_EVENT_LISTENER_HXX
