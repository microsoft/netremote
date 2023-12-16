
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

TEST_CASE("Create an AccessPointDiscoveryAgent instance", "[wifi][core][apdiscoveryagent]")
{
    using namespace Microsoft::Net::Wifi;

    SECTION("Create doesn't cause a crash")
    {
        REQUIRE_NOTHROW(AccessPointDiscoveryAgent{ std::make_unique<Test::AccessPointDiscoveryAgentOperationsTest>() });
    }
}

TEST_CASE("Destroy an AccessPointDiscoveryAgent instance", "[wifi][core][apdiscoveryagent]")
{
    using namespace Microsoft::Net::Wifi;

    auto accessPoint1{ std::make_shared<Test::AccessPointTest>("TestAccessPoint1") };
    auto accessPoint2{ std::make_shared<Test::AccessPointTest>("TestAccessPoint2") };

    SECTION("Destroy doesn't cause a crash")
    {
        auto accessPointDiscoveryAgent{ AccessPointDiscoveryAgent{ std::make_unique<Test::AccessPointDiscoveryAgentOperationsTest>() } };
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.~AccessPointDiscoveryAgent());
    }

    SECTION("Destroy doesn't cause a crash while started")
    {
        auto accessPointDiscoveryAgent{ AccessPointDiscoveryAgent{ std::make_unique<Test::AccessPointDiscoveryAgentOperationsTest>() } };
        accessPointDiscoveryAgent.Start();
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.~AccessPointDiscoveryAgent());
    }

    SECTION("Destroy doesn't cause a crash with discovered access points")
    {
        auto accessPointDiscoveryAgentOperations{ std::make_unique<Test::AccessPointDiscoveryAgentOperationsTest>() };
        auto* accessPointDiscoveryAgentOperationsTest{ accessPointDiscoveryAgentOperations.get() };

        AccessPointDiscoveryAgent accessPointDiscoveryAgent{ std::move(accessPointDiscoveryAgentOperations) };
        accessPointDiscoveryAgentOperationsTest->AddAccessPoint(accessPoint1);
        accessPointDiscoveryAgentOperationsTest->AddAccessPoint(accessPoint2);
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.~AccessPointDiscoveryAgent());
    }

    SECTION("Destroy doesn't cause a crash while started with discovered access points")
    {
        auto accessPointDiscoveryAgentOperations{ std::make_unique<Test::AccessPointDiscoveryAgentOperationsTest>() };
        auto* accessPointDiscoveryAgentOperationsTest{ accessPointDiscoveryAgentOperations.get() };

        AccessPointDiscoveryAgent accessPointDiscoveryAgent{ std::move(accessPointDiscoveryAgentOperations) };
        accessPointDiscoveryAgent.Start();
        accessPointDiscoveryAgentOperationsTest->AddAccessPoint(accessPoint1);
        accessPointDiscoveryAgentOperationsTest->AddAccessPoint(accessPoint2);
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.~AccessPointDiscoveryAgent());
    }

    SECTION("Destory doesn't cause a crash when discovered access point ownership is released")
    {
        auto accessPointDiscoveryAgentOperations{ std::make_unique<Test::AccessPointDiscoveryAgentOperationsTest>() };
        auto* accessPointDiscoveryAgentOperationsTest{ accessPointDiscoveryAgentOperations.get() };

        // Create access points that are exclusively owned by this test.
        auto accessPointOwned1{ std::make_shared<Test::AccessPointTest>("TestAccessPoint1") };
        auto accessPointOwned2{ std::make_shared<Test::AccessPointTest>("TestAccessPoint2") };

        AccessPointDiscoveryAgent accessPointDiscoveryAgent{ std::move(accessPointDiscoveryAgentOperations) };
        accessPointDiscoveryAgent.Start();
        accessPointDiscoveryAgentOperationsTest->AddAccessPoint(accessPointOwned1);
        accessPointDiscoveryAgentOperationsTest->AddAccessPoint(accessPointOwned2);

        // Release test access point ownership.
        accessPointOwned1.reset();
        accessPointOwned2.reset();

        REQUIRE_NOTHROW(accessPointDiscoveryAgent.~AccessPointDiscoveryAgent());
    }
}

TEST_CASE("Start/stop state is reflected correctly", "[wifi][core][apdiscoveryagent]")
{
    using namespace Microsoft::Net::Wifi;

    SECTION("Start/stop state is initially stopped")
    {
        auto accessPointDiscoveryAgent{ AccessPointDiscoveryAgent{ std::make_unique<Test::AccessPointDiscoveryAgentOperationsTest>() } };
        REQUIRE_FALSE(accessPointDiscoveryAgent.IsStarted());
    }

    SECTION("Start/stop state is started after start")
    {
        auto accessPointDiscoveryAgent{ AccessPointDiscoveryAgent{ std::make_unique<Test::AccessPointDiscoveryAgentOperationsTest>() } };
        accessPointDiscoveryAgent.Start();
        REQUIRE(accessPointDiscoveryAgent.IsStarted());
    }

    SECTION("Start/stop state is stopped after stop")
    {
        auto accessPointDiscoveryAgent{ AccessPointDiscoveryAgent{ std::make_unique<Test::AccessPointDiscoveryAgentOperationsTest>() } };
        accessPointDiscoveryAgent.Start();
        accessPointDiscoveryAgent.Stop();
        REQUIRE_FALSE(accessPointDiscoveryAgent.IsStarted());
    }

    SECTION("Start/stop state is started after multiple toggles")
    {
        auto accessPointDiscoveryAgent{ AccessPointDiscoveryAgent{ std::make_unique<Test::AccessPointDiscoveryAgentOperationsTest>() } };
        accessPointDiscoveryAgent.Start();
        accessPointDiscoveryAgent.Stop();
        accessPointDiscoveryAgent.Start();
        accessPointDiscoveryAgent.Stop();
        accessPointDiscoveryAgent.Start();
        REQUIRE(accessPointDiscoveryAgent.IsStarted());
    }

    SECTION("Start/stop state is stopped after multiple toggles")
    {
        auto accessPointDiscoveryAgent{ AccessPointDiscoveryAgent{ std::make_unique<Test::AccessPointDiscoveryAgentOperationsTest>() } };
        accessPointDiscoveryAgent.Start();
        accessPointDiscoveryAgent.Stop();
        accessPointDiscoveryAgent.Start();
        accessPointDiscoveryAgent.Stop();
        REQUIRE_FALSE(accessPointDiscoveryAgent.IsStarted());
    }
}

TEST_CASE("Presence events are raised", "[wifi][core][apdiscoveryagent]")
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

    SECTION("Arrival event is raised after toggling start/stop")
    {
        // Register a callback which completes the promise when invoked.
        accessPointDiscoveryAgent.RegisterDiscoveryEventCallback([&presenceEventRaised](auto&& presenceEvent, auto&& accessPointChanged) {
            presenceEventRaised.set_value({ presenceEvent, std::move(accessPointChanged) });
        });

        // Toggle start/stop state.
        accessPointDiscoveryAgent.Start();
        accessPointDiscoveryAgent.Stop();
        accessPointDiscoveryAgent.Start();

        // Simulate adding an access point and verify the callback fired to ensure prior callback registration was preserved.
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

    SECTION("Departed event is raised after start/stop")
    {
        // Add an initial access point.
        accessPointDiscoveryAgent.Start();
        accessPointDiscoveryAgentOperationsTest->AddAccessPoint(accessPoint);

        // Register a callback which completes the promise when invoked.
        accessPointDiscoveryAgent.RegisterDiscoveryEventCallback([&presenceEventRaised](auto&& presenceEvent, auto&& accessPointChanged) {
            presenceEventRaised.set_value({ presenceEvent, std::move(accessPointChanged) });
        });

        // Toggle start/stop state.
        accessPointDiscoveryAgent.Stop();
        accessPointDiscoveryAgent.Start();

        // Simulate removing an access point and verify the callback fired to ensure prior callback registration was preserved.
        accessPointDiscoveryAgentOperationsTest->RemoveAccessPoint(accessPoint);
        REQUIRE(presenceEventRaisedFuture.wait_for(PresenceEventCallbackWaitTime) == std::future_status::ready);

        // Verify the callback fired with the expected event and access point.
        const auto [presenceEvent, accessPointChanged]{ presenceEventRaisedFuture.get() };
        REQUIRE(presenceEvent == AccessPointPresenceEvent::Departed);
        REQUIRE(accessPointChanged == accessPoint);
    }

    SECTION("Arrival event is not raised when stoppped")
    {
        // Register a callback which completes the promise when invoked.
        accessPointDiscoveryAgent.RegisterDiscoveryEventCallback([&presenceEventRaised](auto&& presenceEvent, auto&& accessPointChanged) {
            presenceEventRaised.set_value({ presenceEvent, std::move(accessPointChanged) });
        });

        // Add an access point and verify the callback was not fired.
        accessPointDiscoveryAgentOperationsTest->AddAccessPoint(accessPoint);
        REQUIRE(presenceEventRaisedFuture.wait_for(PresenceEventCallbackWaitTime) == std::future_status::timeout);
    }
    
    SECTION("Departed event is not raised when stoppped")
    {
        accessPointDiscoveryAgentOperationsTest->AddAccessPoint(accessPoint);

        // Register a callback which completes the promise when invoked.
        accessPointDiscoveryAgent.RegisterDiscoveryEventCallback([&presenceEventRaised](auto&& presenceEvent, auto&& accessPointChanged) {
            presenceEventRaised.set_value({ presenceEvent, std::move(accessPointChanged) });
        });

        // Remove the access point and verify the callback was not fired.
        accessPointDiscoveryAgentOperationsTest->RemoveAccessPoint(accessPoint);
        REQUIRE(presenceEventRaisedFuture.wait_for(PresenceEventCallbackWaitTime) == std::future_status::timeout);
    }
}
