
#include <chrono>
#include <future>
#include <string_view>
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
        REQUIRE_NOTHROW(AccessPointDiscoveryAgent::Create(std::make_unique<Test::AccessPointDiscoveryAgentOperationsTest>()));
    }
}

TEST_CASE("Destroy an AccessPointDiscoveryAgent instance", "[wifi][core][apdiscoveryagent]")
{
    using namespace Microsoft::Net::Wifi;

    std::string_view accessPointInterfaceName1{ "TestAccessPoint1" };
    std::string_view accessPointInterfaceName2{ "TestAccessPoint2" };

    SECTION("Destroy doesn't cause a crash")
    {
        auto accessPointDiscoveryAgent{ AccessPointDiscoveryAgent::Create(std::make_unique<Test::AccessPointDiscoveryAgentOperationsTest>()) };
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.reset());
    }

    SECTION("Destroy doesn't cause a crash while started")
    {
        auto accessPointDiscoveryAgent{ AccessPointDiscoveryAgent::Create(std::make_unique<Test::AccessPointDiscoveryAgentOperationsTest>()) };
        accessPointDiscoveryAgent->Start();
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.reset());
    }

    SECTION("Destroy doesn't cause a crash with discovered access points")
    {
        auto accessPointDiscoveryAgentOperations{ std::make_unique<Test::AccessPointDiscoveryAgentOperationsTest>() };
        auto* accessPointDiscoveryAgentOperationsTest{ accessPointDiscoveryAgentOperations.get() };

        auto accessPointDiscoveryAgent{ AccessPointDiscoveryAgent::Create(std::move(accessPointDiscoveryAgentOperations)) };
        accessPointDiscoveryAgentOperationsTest->AddAccessPoint(accessPointInterfaceName1);
        accessPointDiscoveryAgentOperationsTest->AddAccessPoint(accessPointInterfaceName2);
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.reset());
    }

    SECTION("Destroy doesn't cause a crash while started with discovered access points")
    {
        auto accessPointDiscoveryAgentOperations{ std::make_unique<Test::AccessPointDiscoveryAgentOperationsTest>() };
        auto* accessPointDiscoveryAgentOperationsTest{ accessPointDiscoveryAgentOperations.get() };

        auto accessPointDiscoveryAgent{ AccessPointDiscoveryAgent::Create(std::move(accessPointDiscoveryAgentOperations)) };
        accessPointDiscoveryAgent->Start();
        accessPointDiscoveryAgentOperationsTest->AddAccessPoint(accessPointInterfaceName1);
        accessPointDiscoveryAgentOperationsTest->AddAccessPoint(accessPointInterfaceName2);
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.reset());
    }
}

TEST_CASE("Start/stop state is reflected correctly", "[wifi][core][apdiscoveryagent]")
{
    using namespace Microsoft::Net::Wifi;

    SECTION("Start/stop state is initially stopped")
    {
        auto accessPointDiscoveryAgent{ AccessPointDiscoveryAgent::Create(std::make_unique<Test::AccessPointDiscoveryAgentOperationsTest>()) };
        REQUIRE_FALSE(accessPointDiscoveryAgent->IsStarted());
    }

    SECTION("Start/stop state is started after start")
    {
        auto accessPointDiscoveryAgent{ AccessPointDiscoveryAgent::Create(std::make_unique<Test::AccessPointDiscoveryAgentOperationsTest>()) };
        accessPointDiscoveryAgent->Start();
        REQUIRE(accessPointDiscoveryAgent->IsStarted());
    }

    SECTION("Start/stop state is stopped after stop")
    {
        auto accessPointDiscoveryAgent{ AccessPointDiscoveryAgent::Create(std::make_unique<Test::AccessPointDiscoveryAgentOperationsTest>()) };
        accessPointDiscoveryAgent->Start();
        accessPointDiscoveryAgent->Stop();
        REQUIRE_FALSE(accessPointDiscoveryAgent->IsStarted());
    }

    SECTION("Start/stop state is started after multiple toggles")
    {
        auto accessPointDiscoveryAgent{ AccessPointDiscoveryAgent::Create(std::make_unique<Test::AccessPointDiscoveryAgentOperationsTest>()) };
        accessPointDiscoveryAgent->Start();
        accessPointDiscoveryAgent->Stop();
        accessPointDiscoveryAgent->Start();
        accessPointDiscoveryAgent->Stop();
        accessPointDiscoveryAgent->Start();
        REQUIRE(accessPointDiscoveryAgent->IsStarted());
    }

    SECTION("Start/stop state is stopped after multiple toggles")
    {
        auto accessPointDiscoveryAgent{ AccessPointDiscoveryAgent::Create(std::make_unique<Test::AccessPointDiscoveryAgentOperationsTest>()) };
        accessPointDiscoveryAgent->Start();
        accessPointDiscoveryAgent->Stop();
        accessPointDiscoveryAgent->Start();
        accessPointDiscoveryAgent->Stop();
        REQUIRE_FALSE(accessPointDiscoveryAgent->IsStarted());
    }
}

TEST_CASE("Presence events are raised", "[wifi][core][apdiscoveryagent]")
{
    using namespace Microsoft::Net::Wifi;

    static constexpr auto PresenceEventCallbackWaitTime{ 100ms };

    std::string_view accessPointInterfaceName{ "TestAccessPoint" };

    // Create an instance of test discovery agent operations.
    auto accessPointDiscoveryAgentOperations{ std::make_unique<Test::AccessPointDiscoveryAgentOperationsTest>() };
    auto* accessPointDiscoveryAgentOperationsTest{ accessPointDiscoveryAgentOperations.get() };

    // Create a discovery agent using the test operations.
    auto accessPointDiscoveryAgent{ AccessPointDiscoveryAgent::Create(std::move(accessPointDiscoveryAgentOperations)) };

    // Create a promise to capture the presence event callback invocation data.
    std::promise<std::pair<AccessPointPresenceEvent, std::string>> presenceEventRaised;
    auto presenceEventRaisedFuture{ presenceEventRaised.get_future() };

    SECTION("Arrival event is raised")
    {
        // Register a callback which completes the promise when invoked.
        accessPointDiscoveryAgent->RegisterDiscoveryEventCallback([&presenceEventRaised](auto&& presenceEvent, auto&& accessPointChanged) {
            presenceEventRaised.set_value({ presenceEvent, std::move(accessPointChanged) });
        });

        accessPointDiscoveryAgent->Start();

        // Simulate adding an access point and verify the callback fired.
        accessPointDiscoveryAgentOperationsTest->AddAccessPoint(accessPointInterfaceName);
        REQUIRE(presenceEventRaisedFuture.wait_for(PresenceEventCallbackWaitTime) == std::future_status::ready);

        // Verify the callback fired with the expected event and access point.
        const auto [presenceEvent, accessPointInterfaceNameChanged]{ presenceEventRaisedFuture.get() };
        REQUIRE(presenceEvent == AccessPointPresenceEvent::Arrived);
        REQUIRE(accessPointInterfaceNameChanged == accessPointInterfaceName);
    }

    SECTION("Arrival event is raised after toggling start/stop")
    {
        // Register a callback which completes the promise when invoked.
        accessPointDiscoveryAgent->RegisterDiscoveryEventCallback([&presenceEventRaised](auto&& presenceEvent, auto&& accessPointChanged) {
            presenceEventRaised.set_value({ presenceEvent, std::move(accessPointChanged) });
        });

        // Toggle start/stop state.
        accessPointDiscoveryAgent->Start();
        accessPointDiscoveryAgent->Stop();
        accessPointDiscoveryAgent->Start();

        // Simulate adding an access point and verify the callback fired to ensure prior callback registration was preserved.
        accessPointDiscoveryAgentOperationsTest->AddAccessPoint(accessPointInterfaceName);
        REQUIRE(presenceEventRaisedFuture.wait_for(PresenceEventCallbackWaitTime) == std::future_status::ready);

        // Verify the callback fired with the expected event and access point.
        const auto [presenceEvent, accessPointInterfaceNameChanged]{ presenceEventRaisedFuture.get() };
        REQUIRE(presenceEvent == AccessPointPresenceEvent::Arrived);
        REQUIRE(accessPointInterfaceNameChanged == accessPointInterfaceName);
    }

    SECTION("Departed event is raised")
    {
        // Add an initial access point.
        accessPointDiscoveryAgent->Start();
        accessPointDiscoveryAgentOperationsTest->AddAccessPoint(accessPointInterfaceName);

        // Register a callback which completes the promise when invoked.
        accessPointDiscoveryAgent->RegisterDiscoveryEventCallback([&presenceEventRaised](auto&& presenceEvent, auto&& accessPointChanged) {
            presenceEventRaised.set_value({ presenceEvent, std::move(accessPointChanged) });
        });

        // Simulate removing an access point and verify the callback fired.
        accessPointDiscoveryAgentOperationsTest->RemoveAccessPoint(accessPointInterfaceName);
        REQUIRE(presenceEventRaisedFuture.wait_for(PresenceEventCallbackWaitTime) == std::future_status::ready);

        // Verify the callback fired with the expected event and access point.
        const auto [presenceEvent, accessPointInterfaceNameChanged]{ presenceEventRaisedFuture.get() };
        REQUIRE(presenceEvent == AccessPointPresenceEvent::Departed);
        REQUIRE(accessPointInterfaceNameChanged == accessPointInterfaceName);
    }

    SECTION("Departed event is raised after start/stop")
    {
        // Add an initial access point.
        accessPointDiscoveryAgent->Start();
        accessPointDiscoveryAgentOperationsTest->AddAccessPoint(accessPointInterfaceName);

        // Register a callback which completes the promise when invoked.
        accessPointDiscoveryAgent->RegisterDiscoveryEventCallback([&presenceEventRaised](auto&& presenceEvent, auto&& accessPointChanged) {
            presenceEventRaised.set_value({ presenceEvent, std::move(accessPointChanged) });
        });

        // Toggle start/stop state.
        accessPointDiscoveryAgent->Stop();
        accessPointDiscoveryAgent->Start();

        // Simulate removing an access point and verify the callback fired to ensure prior callback registration was preserved.
        accessPointDiscoveryAgentOperationsTest->RemoveAccessPoint(accessPointInterfaceName);
        REQUIRE(presenceEventRaisedFuture.wait_for(PresenceEventCallbackWaitTime) == std::future_status::ready);

        // Verify the callback fired with the expected event and access point.
        const auto [presenceEvent, accessPointInterfaceNameChanged]{ presenceEventRaisedFuture.get() };
        REQUIRE(presenceEvent == AccessPointPresenceEvent::Departed);
        REQUIRE(accessPointInterfaceNameChanged == accessPointInterfaceName);
    }

    SECTION("Arrival event is not raised when stoppped")
    {
        // Register a callback which completes the promise when invoked.
        accessPointDiscoveryAgent->RegisterDiscoveryEventCallback([&presenceEventRaised](auto&& presenceEvent, auto&& accessPointChanged) {
            presenceEventRaised.set_value({ presenceEvent, std::move(accessPointChanged) });
        });

        // Add an access point and verify the callback was not fired.
        accessPointDiscoveryAgentOperationsTest->AddAccessPoint(accessPointInterfaceName);
        REQUIRE(presenceEventRaisedFuture.wait_for(PresenceEventCallbackWaitTime) == std::future_status::timeout);
    }

    SECTION("Departed event is not raised when stoppped")
    {
        accessPointDiscoveryAgentOperationsTest->AddAccessPoint(accessPointInterfaceName);

        // Register a callback which completes the promise when invoked.
        accessPointDiscoveryAgent->RegisterDiscoveryEventCallback([&presenceEventRaised](auto&& presenceEvent, auto&& accessPointChanged) {
            presenceEventRaised.set_value({ presenceEvent, std::move(accessPointChanged) });
        });

        // Remove the access point and verify the callback was not fired.
        accessPointDiscoveryAgentOperationsTest->RemoveAccessPoint(accessPointInterfaceName);
        REQUIRE(presenceEventRaisedFuture.wait_for(PresenceEventCallbackWaitTime) == std::future_status::timeout);
    }
}
