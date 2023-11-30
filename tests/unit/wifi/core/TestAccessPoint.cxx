
#include <optional>

#include <catch2/catch_test_macros.hpp>
#include <microsoft/net/wifi/AccessPoint.hxx>
#include <microsoft/net/wifi/test/AccessPointFactoryTest.hxx>

namespace Microsoft::Net::Wifi::Test
{
static constexpr auto InterfaceNameDefault{ "wlan0" };
} // namespace Microsoft::Net::Wifi::Test

TEST_CASE("Create an AccessPoint instance", "[wifi][core][ap]")
{
    using namespace Microsoft::Net::Wifi;

    SECTION("Create doesn't cause a crash")
    {
        std::optional<AccessPoint> accessPoint;
        REQUIRE_NOTHROW(accessPoint.emplace(Test::InterfaceNameDefault));
    }

    SECTION("Create multiple for same interface doesn't cause a crash")
    {
        std::optional<AccessPoint> accessPoint1;
        REQUIRE_NOTHROW(accessPoint1.emplace(Test::InterfaceNameDefault));

        std::optional<AccessPoint> accessPoint2;
        REQUIRE_NOTHROW(accessPoint2.emplace(Test::InterfaceNameDefault));
    }
}

TEST_CASE("Destroy an AccessPoint instance", "[wifi][core][ap]")
{
    using namespace Microsoft::Net::Wifi;

    SECTION("Destroy doesn't cause a crash")
    {
        std::optional<AccessPoint> accessPoint(Test::InterfaceNameDefault);
        REQUIRE_NOTHROW(accessPoint.reset());
    }
}

TEST_CASE("AccessPoint instance reflects basic properties", "[wifi][core][ap]")
{
    using namespace Microsoft::Net::Wifi;

    SECTION("GetInterface() returns the interface name")
    {
        AccessPoint accessPoint{ Test::InterfaceNameDefault };
        REQUIRE(accessPoint.GetInterface() == Test::InterfaceNameDefault);
    }
}
