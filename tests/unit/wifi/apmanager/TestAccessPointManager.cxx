
#include <catch2/catch_test_macros.hpp>
#include <microsoft/net/wifi/AccessPointDiscoveryAgent.hxx>
#include <microsoft/net/wifi/AccessPointManager.hxx>

#include "AccessPointDiscoveryAgentOperationsTest.hxx"

TEST_CASE("Create an AccessPointManager instance", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;

    SECTION("Create doesn't cause a crash")
    {
        REQUIRE_NOTHROW(AccessPointManager::Create());
    }
}

TEST_CASE("Destroy an AccessPointManager instance", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;

    using Test::AccessPointDiscoveryAgentOperationsTest;

    SECTION("Destroy doesn't cause a crash")
    {
        auto accessPointManager{ AccessPointManager::Create() };
        REQUIRE_NOTHROW(accessPointManager.reset());
    }

    SECTION("Destroy doesn't cause a crash when there are registered discovery agents")
    {
        auto accessPointDiscoveryAgent1{ AccessPointDiscoveryAgent::Create(std::make_unique<AccessPointDiscoveryAgentOperationsTest>()) };
        auto accessPointDiscoveryAgent2{ AccessPointDiscoveryAgent::Create(std::make_unique<AccessPointDiscoveryAgentOperationsTest>()) };

        auto accessPointManager{ AccessPointManager::Create() };
        accessPointManager->AddDiscoveryAgent(std::move(accessPointDiscoveryAgent1));
        accessPointManager->AddDiscoveryAgent(std::move(accessPointDiscoveryAgent2));
        REQUIRE_NOTHROW(accessPointManager.reset());
    }
}

TEST_CASE("AccessPointManager instance reflects basic properties", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;

    SECTION("GetAllAccessPoints() returns an empty list")
    {
        auto accessPointManager{ AccessPointManager::Create() };
        REQUIRE(std::empty(accessPointManager->GetAllAccessPoints()));
    }
}
