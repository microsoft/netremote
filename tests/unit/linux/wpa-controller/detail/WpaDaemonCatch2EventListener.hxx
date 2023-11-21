
#ifndef WPA_DAEMON_CATCH2_EVENT_LISTENER_HXX
#define WPA_DAEMON_CATCH2_EVENT_LISTENER_HXX

#include <memory>
#include <unordered_map>

#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>
#include <Wpa/WpaCore.hxx>

#include "WifiVirtualDeviceManager.hxx"
#include "WpaDaemonInstance.hxx"

struct WpaDaemonCatch2EventListener : public Catch::EventListenerBase
{
    // Inherit base class constructor.
    using EventListenerBase::EventListenerBase; 

    /**
     * @brief Runs when a test case is started.
     * 
     * @param testInfo 
     */
    void testCaseStarting(Catch::TestCaseInfo const& testInfo) override;

    /**
     * @brief Runs when the last test case has ended.
     * 
     * @param testCaseStats 
     * @return * void 
     */
    void testCaseEnded(Catch::TestCaseStats const& testCaseStats) override;

private:
    std::unordered_map<Wpa::WpaType, std::unique_ptr<IWpaDaemonInstance>> m_wpaDaemonInstances;
    WifiVirtualDeviceManager m_wifiVirtualDeviceManager;
};

CATCH_REGISTER_LISTENER(WpaDaemonCatch2EventListener)

#endif // WPA_DAEMON_CATCH2_EVENT_LISTENER_HXX
