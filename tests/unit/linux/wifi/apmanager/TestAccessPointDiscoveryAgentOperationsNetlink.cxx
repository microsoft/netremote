
#include <optional>
#include <utility>

#include <catch2/catch_test_macros.hpp>

#include <microsoft/net/wifi/AccessPointDiscoveryAgentOperationsNetlink.hxx>
#include <microsoft/net/wifi/test/AccessPointTest.hxx>
#include <microsoft/net/wifi/test/AccessPointFactoryLinuxTest.hxx>

TEST_CASE("Create AccessPointDiscoveryAgentOperationsNetlink", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;
    using namespace Microsoft::Net::Wifi::Test;

    SECTION("Create doesn't cause a crash")
    {
        REQUIRE_NOTHROW(AccessPointDiscoveryAgentOperationsNetlink{ std::make_shared<AccessPointFactoryLinuxTest>() });
    }
}

TEST_CASE("Destroy AccessPointDiscoveryAgentOperationsNetlink", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;
    using namespace Microsoft::Net::Wifi::Test;

    SECTION("Destroy doesn't cause a crash")
    {
        std::optional<AccessPointDiscoveryAgentOperationsNetlink> accessPointDiscoveryAgent(std::make_shared<AccessPointFactoryLinuxTest>());
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.reset());
    }
}

TEST_CASE("AccessPointDiscoveryAgentOperationsNetlink::Start", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;
    using namespace Microsoft::Net::Wifi::Test;

    SECTION("Start doesn't cause a crash")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent{ std::make_shared<AccessPointFactoryLinuxTest>() };
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.Start([](auto &&, auto &&) {}));
    }

    SECTION("Start doesn't cause a crash when called twice")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent{ std::make_shared<AccessPointFactoryLinuxTest>() };
        accessPointDiscoveryAgent.Start([](auto &&, auto &&) {});
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.Start([](auto &&, auto &&) {}));
    }

    SECTION("Start doesn't cause a crash when called with a null callback")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent{ std::make_shared<AccessPointFactoryLinuxTest>() };
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.Start(nullptr));
    }
}

TEST_CASE("AccessPointDiscoveryAgentOperationsNetlink::Stop", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;
    using namespace Microsoft::Net::Wifi::Test;

    SECTION("Stop doesn't cause a crash when Start hasn't been called")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent{ std::make_shared<AccessPointFactoryLinuxTest>() };
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.Stop());
    }

    SECTION("Stop doesn't cause a crash when Start has been called")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent{ std::make_shared<AccessPointFactoryLinuxTest>() };
        accessPointDiscoveryAgent.Start([](auto &&, auto &&) {});
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.Stop());
    }

    SECTION("Stop doesn't cause a crash when called twice")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent{ std::make_shared<AccessPointFactoryLinuxTest>() };
        accessPointDiscoveryAgent.Start([](auto &&, auto &&) {});
        accessPointDiscoveryAgent.Stop();
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.Stop());
    }

    SECTION("Stop doesn't cause a crash when called with a null callback")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent{ std::make_shared<AccessPointFactoryLinuxTest>() };
        accessPointDiscoveryAgent.Start(nullptr);
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.Stop());
    }
}

TEST_CASE("AccessPointDiscoveryAgentOperationsNetlink::ProbeAsync", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;
    using namespace Microsoft::Net::Wifi::Test;

    SECTION("ProbeAsync doesn't cause a crash when Start hasn't been called")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent{ std::make_shared<AccessPointFactoryLinuxTest>() };
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.ProbeAsync());
    }

    SECTION("ProbeAsync doesn't cause a crash when Start has been called")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent{ std::make_shared<AccessPointFactoryLinuxTest>() };
        accessPointDiscoveryAgent.Start([](auto &&, auto &&) {});
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.ProbeAsync());
    }

    SECTION("ProbeAsync doesn't cause a crash when Stop has been called")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent{ std::make_shared<AccessPointFactoryLinuxTest>() };
        accessPointDiscoveryAgent.Stop();
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.ProbeAsync());
    }

    SECTION("ProbeAsync doesn't cause a crash when called twice")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent{ std::make_shared<AccessPointFactoryLinuxTest>() };
        accessPointDiscoveryAgent.Start([](auto &&, auto &&) {});
        accessPointDiscoveryAgent.ProbeAsync();
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.ProbeAsync());
    }

    SECTION("ProbeAsync doesn't cause a crash when called after a Start/Stop sequence")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent{ std::make_shared<AccessPointFactoryLinuxTest>() };
        accessPointDiscoveryAgent.Start(nullptr);
        accessPointDiscoveryAgent.Stop();
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.ProbeAsync());
    }

    SECTION("ProbeAsync returns a valid future")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent{ std::make_shared<AccessPointFactoryLinuxTest>() };
        accessPointDiscoveryAgent.Start(nullptr);
        REQUIRE(accessPointDiscoveryAgent.ProbeAsync().valid());
    }

    SECTION("ProbeAsync result can be obtained without causing a crash")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent{ std::make_shared<AccessPointFactoryLinuxTest>() };
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.ProbeAsync().get());
    }

    SECTION("ProbeAsync result is valid")
    {
        AccessPointDiscoveryAgentOperationsNetlink accessPointDiscoveryAgent{ std::make_shared<AccessPointFactoryLinuxTest>() };
        REQUIRE_NOTHROW(accessPointDiscoveryAgent.ProbeAsync().get().clear());
    }
}
