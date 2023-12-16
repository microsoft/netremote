
#include <chrono>
#include <future>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <microsoft/net/wifi/AccessPointDiscoveryAgent.hxx>
#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPointDiscoveryAgentOperations.hxx>

#include "AccessPointDiscoveryAgentOperationsTest.hxx"
#include "AccessPointTest.hxx"

using namespace Microsoft::Net::Wifi;
using namespace std::chrono_literals;

TEST_CASE("Create an AccessPointDiscoveryAgent instance", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;

    SECTION("Create doesn't cause a crash")
    {
        REQUIRE_NOTHROW(AccessPointDiscoveryAgent{ std::make_unique<Test::AccessPointDiscoveryAgentOperationsTest>() });
    }
}

TEST_CASE("Destroy an AccessPointDiscoveryAgent instance", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;

    SECTION("Destroy doesn't cause a crash")
    {
        auto accessPointDiscoveryAgent{ AccessPointDiscoveryAgent{ std::make_unique<Test::AccessPointDiscoveryAgentOperationsTest>() } };
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.~AccessPointDiscoveryAgent());
    }
}

TEST_CASE("Presence events are raised", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;

    static constexpr auto PresenceEventCallbackWaitTime{ 100ms };

    auto accessPoint{ std::make_shared<Test::AccessPointTest>("TestAccessPoint") };

    // Create an instance of test discovery agent operations.
    auto accessPointDiscoveryAgentOperations{ std::make_unique<Test::AccessPointDiscoveryAgentOperationsTest>() };
    auto* accessPointDiscoveryAgentOperationsTest{ accessPointDiscoveryAgentOperations.get() };

    // Create a discovery agent using the test operations.
    auto accessPointDiscoveryAgent{ AccessPointDiscoveryAgent{ std::move(accessPointDiscoveryAgentOperations) } };

    // Create a promise to capture the presence event callback invocation data.
    std::promise<std::pair<AccessPointPresenceEvent, std::shared_ptr<IAccessPoint>>> presenceEventRaised;
    auto presenceEventRaisedFuture{ presenceEventRaised.get_future() };

    SECTION("Arrival event is raised")
    {
        // Register a callback which completes the promise when invoked.
        accessPointDiscoveryAgent.RegisterDiscoveryEventCallback([&presenceEventRaised](auto&& presenceEvent, auto&& accessPointChanged) {
            presenceEventRaised.set_value({ presenceEvent, std::move(accessPointChanged) });
        });

        accessPointDiscoveryAgent.Start();

        // Simulate adding an access point and verify the callback fired.
        accessPointDiscoveryAgentOperationsTest->AddAccessPoint(accessPoint);
        REQUIRE(presenceEventRaisedFuture.wait_for(PresenceEventCallbackWaitTime) == std::future_status::ready);

        // Verify the callback fired with the expected event and access point.
        const auto [presenceEvent, accessPointChanged]{ presenceEventRaisedFuture.get() };
        REQUIRE(presenceEvent == AccessPointPresenceEvent::Arrived);
        REQUIRE(accessPointChanged == accessPoint);
    }

    SECTION("Departed event is raised")
    {
        // Add an initial access point.
        accessPointDiscoveryAgent.Start();
        accessPointDiscoveryAgentOperationsTest->AddAccessPoint(accessPoint);

        // Register a callback which completes the promise when invoked.
        accessPointDiscoveryAgent.RegisterDiscoveryEventCallback([&presenceEventRaised](auto&& presenceEvent, auto&& accessPointChanged) {
            presenceEventRaised.set_value({ presenceEvent, std::move(accessPointChanged) });
        });

        // Simulate removing an access point and verify the callback fired.
        accessPointDiscoveryAgentOperationsTest->RemoveAccessPoint(accessPoint);
        REQUIRE(presenceEventRaisedFuture.wait_for(PresenceEventCallbackWaitTime) == std::future_status::ready);

        // Verify the callback fired with the expected event and access point.
        const auto [presenceEvent, accessPointChanged]{ presenceEventRaisedFuture.get() };
        REQUIRE(presenceEvent == AccessPointPresenceEvent::Departed);
        REQUIRE(accessPointChanged == accessPoint);
    }
}
