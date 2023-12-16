
#include <catch2/catch_test_macros.hpp>
#include <microsoft/net/wifi/AccessPointManager.hxx>

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

    SECTION("Destroy doesn't cause a crash")
    {
        auto accessPointManager{ AccessPointManager::Create() };
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
