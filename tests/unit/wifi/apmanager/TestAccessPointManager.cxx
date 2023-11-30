
#include <catch2/catch_test_macros.hpp>
#include <microsoft/net/wifi/AccessPointManager.hxx>
#include <microsoft/net/wifi/test/AccessPointFactoryTest.hxx>

TEST_CASE("Create an AccessPointManager instance", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;

    auto accessPointFactory = std::make_unique<Test::AccessPointFactoryTest>();

    SECTION("Create doesn't cause a crash")
    {
        REQUIRE_NOTHROW(AccessPointManager{ std::move(accessPointFactory) });
    }
}

TEST_CASE("Destroy an AccessPointManager instance", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;

    auto accessPointFactory = std::make_unique<Test::AccessPointFactoryTest>();

    SECTION("Destroy doesn't cause a crash")
    {
        AccessPointManager accessPointManager{ std::move(accessPointFactory) };
        REQUIRE_NOTHROW(accessPointManager.~AccessPointManager());
    }
}

TEST_CASE("AccessPointManager instance reflects basic properties", "[wifi][core][apmanager]")
{
    using namespace Microsoft::Net::Wifi;

    auto accessPointFactory = std::make_unique<Test::AccessPointFactoryTest>();

    SECTION("EnumerateAccessPoints() returns an empty list")
    {
        AccessPointManager accessPointManager{ std::move(accessPointFactory) };
        REQUIRE(std::empty(accessPointManager.EnumerateAccessPoints()));
    }
}
