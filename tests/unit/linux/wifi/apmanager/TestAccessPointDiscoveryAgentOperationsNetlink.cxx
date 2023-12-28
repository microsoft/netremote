
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
