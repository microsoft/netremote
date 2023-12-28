
#include <optional>
#include <utility>

#include <catch2/catch_test_macros.hpp>

#include <microsoft/net/wifi/AccessPointDiscoveryAgentOperationsNetlink.hxx>

TEST_CASE("Create AccessPointDiscoveryAgentOperationsNetlink", "[wifi][core][apmanager]")
{
    SECTION("Create doesn't cause a crash")
    {
        using namespace Microsoft::Net::Wifi;

        REQUIRE_NOTHROW(AccessPointDiscoveryAgentOperationsNetlink{});
    }
}

TEST_CASE("Destroy AccessPointDiscoveryAgentOperationsNetlink", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;

    SECTION("Destroy doesn't cause a crash")
    {
        std::optional<AccessPointDiscoveryAgentOperationsNetlink> accessPointDiscoveryAgent(std::in_place);
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.reset());
    }
}

TEST_CASE("AccessPointDiscoveryAgentOperationsNetlink::Start", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;

    SECTION("Start doesn't cause a crash")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent;
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.Start([](auto &&, auto &&) {}));
    }

    SECTION("Start doesn't cause a crash when called twice")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent;
        accessPointDiscoveryAgent.Start([](auto &&, auto &&) {});
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.Start([](auto &&, auto &&) {}));
    }

    SECTION("Start doesn't cause a crash when called with a null callback")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent;
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.Start(nullptr));
    }
}

TEST_CASE("AccessPointDiscoveryAgentOperationsNetlink::Stop", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;

    SECTION("Stop doesn't cause a crash when Start hasn't been called")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent{};
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.Stop());
    }

    SECTION("Stop doesn't cause a crash when Start has been called")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent{};
        accessPointDiscoveryAgent.Start([](auto &&, auto &&) {});
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.Stop());
    }

    SECTION("Stop doesn't cause a crash when called twice")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent{};
        accessPointDiscoveryAgent.Start([](auto &&, auto &&) {});
        accessPointDiscoveryAgent.Stop();
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.Stop());
    }

    SECTION("Stop doesn't cause a crash when called with a null callback")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent{};
        accessPointDiscoveryAgent.Start(nullptr);
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.Stop());
    }
}

TEST_CASE("AccessPointDiscoveryAgentOperationsNetlink::ProbeAsync", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;

    SECTION("ProbeAsync doesn't cause a crash when Start hasn't been called")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent{};
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.ProbeAsync());
    }

    SECTION("ProbeAsync doesn't cause a crash when Start has been called")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent{};
        accessPointDiscoveryAgent.Start([](auto &&, auto &&) {});
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.ProbeAsync());
    }

    SECTION("ProbeAsync doesn't cause a crash when Stop has been called")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent{};
        accessPointDiscoveryAgent.Stop();
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.ProbeAsync());
    }

    SECTION("ProbeAsync doesn't cause a crash when called twice")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent{};
        accessPointDiscoveryAgent.Start([](auto &&, auto &&) {});
        accessPointDiscoveryAgent.ProbeAsync();
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.ProbeAsync());
    }

    SECTION("ProbeAsync doesn't cause a crash when called after a Start/Stop sequence")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent{};
        accessPointDiscoveryAgent.Start(nullptr);
        accessPointDiscoveryAgent.Stop();
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.ProbeAsync());
    }
}
