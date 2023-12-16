
#include <catch2/catch_test_macros.hpp>
#include <microsoft/net/wifi/AccessPointFactoryLinux.hxx>

namespace Microsoft::Net::Wifi::Test
{
static constexpr auto InterfaceNameDefault{ "wlan0" };
} // namespace Microsoft::Net::Wifi::Test

TEST_CASE("Create an AccessPointFactoryLinux instance", "[wifi][core][ap][linux]")
{
    using namespace Microsoft::Net::Wifi;

    SECTION("Create doesn't cause a crash")
    {
        REQUIRE_NOTHROW(AccessPointFactoryLinux{});
    }

    SECTION("Create multiple doesn't cause a crash")
    {
        REQUIRE_NOTHROW(AccessPointFactoryLinux{});
        REQUIRE_NOTHROW(AccessPointFactoryLinux{});
    }
}

TEST_CASE("Destroy an AccessPointFactoryLinux instance", "[wifi][core][ap][linux]")
{
    using namespace Microsoft::Net::Wifi;

    SECTION("Destroy doesn't cause a crash")
    {
        AccessPointFactoryLinux accessPointFactory{};
        REQUIRE_NOTHROW(accessPointFactory.~AccessPointFactoryLinux());
    }
}

TEST_CASE("Create() function", "[wifi][core][ap][linux]")
{
    using namespace Microsoft::Net::Wifi;

    SECTION("Create() doesn't cause a crash")
    {
        AccessPointFactoryLinux accessPointFactory{};
        REQUIRE_NOTHROW(accessPointFactory.Create(Test::InterfaceNameDefault));
    }

    SECTION("Create() with empty/null inteface doesn't cause a crash")
    {
        AccessPointFactoryLinux accessPointFactory{};
        REQUIRE_NOTHROW(accessPointFactory.Create({}));
    }

    SECTION("Create() with valid input arguments returns non-nullptr instance")
    {
        AccessPointFactoryLinux accessPointFactory{};
        REQUIRE(accessPointFactory.Create(Test::InterfaceNameDefault) != nullptr);
    }

    SECTION("Create() instance reflects basic properties from base class")
    {
        AccessPointFactoryLinux accessPointFactory{};
        const auto accessPoint = accessPointFactory.Create(Test::InterfaceNameDefault);
        REQUIRE(accessPoint->GetInterface() == Test::InterfaceNameDefault);
    }
}
