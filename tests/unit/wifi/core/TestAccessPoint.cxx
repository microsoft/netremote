
#include <optional>
#include <stdexcept>

#include <catch2/catch_test_macros.hpp>
#include <microsoft/net/wifi/AccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPointController.hxx>
#include <microsoft/net/wifi/test/AccessPointTest.hxx>
#include <microsoft/net/wifi/test/AccessPointControllerTest.hxx>

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
        REQUIRE_NOTHROW(accessPoint.emplace(Test::InterfaceNameDefault, std::make_unique<Test::AccessPointControllerFactoryTest>()));
    }

    SECTION("Create multiple for same interface doesn't cause a crash")
    {
        std::optional<AccessPoint> accessPoint1;
        REQUIRE_NOTHROW(accessPoint1.emplace(Test::InterfaceNameDefault, std::make_unique<Test::AccessPointControllerFactoryTest>()));

        std::optional<AccessPoint> accessPoint2;
        REQUIRE_NOTHROW(accessPoint2.emplace(Test::InterfaceNameDefault, std::make_unique<Test::AccessPointControllerFactoryTest>()));
    }
}

TEST_CASE("Destroy an AccessPoint instance", "[wifi][core][ap]")
{
    using namespace Microsoft::Net::Wifi;

    SECTION("Destroy doesn't cause a crash")
    {
        AccessPoint accessPoint{ Test::InterfaceNameDefault, std::make_unique<Test::AccessPointControllerFactoryTest>() };
        REQUIRE_NOTHROW(accessPoint.~AccessPoint());
    }
}

TEST_CASE("AccessPoint instance reflects basic properties", "[wifi][core][ap]")
{
    using namespace Microsoft::Net::Wifi;

    SECTION("GetInterface() returns the interface name")
    {
        AccessPoint accessPoint{ Test::InterfaceNameDefault, std::make_unique<Test::AccessPointControllerFactoryTest>() };
        REQUIRE(accessPoint.GetInterface() == Test::InterfaceNameDefault);
    }
}
