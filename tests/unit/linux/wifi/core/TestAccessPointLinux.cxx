
#include <memory>

#include <catch2/catch_test_macros.hpp>
#include <microsoft/net/wifi/AccessPointLinux.hxx>
#include <microsoft/net/wifi/test/AccessPointControllerTest.hxx>

namespace Microsoft::Net::Wifi::Test
{
static constexpr auto InterfaceNameDefault{ "wlan0" };
} // namespace Microsoft::Net::Wifi::Test

TEST_CASE("Create an AccessPointLinux instance", "[wifi][core][ap][linux]")
{
    using namespace Microsoft::Net::Wifi;

    SECTION("Create doesn't cause a crash with null controller factory")
    {
        std::optional<AccessPointLinux> accessPoint;
        REQUIRE_NOTHROW(accessPoint.emplace(Test::InterfaceNameDefault, nullptr));
    }

    SECTION("Create doesn't cause a crash")
    {
        std::optional<AccessPointLinux> accessPoint;
        REQUIRE_NOTHROW(accessPoint.emplace(Test::InterfaceNameDefault, std::make_unique<Test::AccessPointControllerFactoryTest>()));
    }

    SECTION("Create multiple for same interface doesn't cause a crash")
    {
        std::optional<AccessPointLinux> accessPoint1;
        REQUIRE_NOTHROW(accessPoint1.emplace(Test::InterfaceNameDefault, std::make_unique<Test::AccessPointControllerFactoryTest>()));

        std::optional<AccessPointLinux> accessPoint2;
        REQUIRE_NOTHROW(accessPoint2.emplace(Test::InterfaceNameDefault, std::make_unique<Test::AccessPointControllerFactoryTest>()));
    }
}
