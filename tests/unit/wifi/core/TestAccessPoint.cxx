
#include <optional>

#include <catch2/catch_test_macros.hpp>
#include <microsoft/net/wifi/AccessPoint.hxx>

#include "detail/AccessPointFactoryTest.hxx"

namespace Microsoft::Net::Wifi::Test
{
static constexpr auto InterfaceNameDefault{ "wlan0" };
} // namespace Microsoft::Net::Wifi::Test

TEST_CASE("Create an AccessPoint instance", "[wifi][core]")
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
