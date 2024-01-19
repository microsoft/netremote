
#include <catch2/catch_test_macros.hpp>
#include <microsoft/net/wifi/AccessPointLinux.hxx>
#include <microsoft/net/wifi/test/AccessPointControllerTest.hxx>

namespace Microsoft::Net::Wifi::Test
{
static constexpr auto InterfaceNameDefault{ "wlan0" };
} // namespace Microsoft::Net::Wifi::Test

TEST_CASE("Create an AccessPointFactoryLinux instance", "[wifi][core][ap][linux]")
{
    using namespace Microsoft::Net::Wifi;

    SECTION("Create doesn't cause a crash with null controller factory")
    {
        REQUIRE_NOTHROW(AccessPointFactoryLinux{ nullptr });
    }

    SECTION("Create doesn't cause a crash")
    {
        REQUIRE_NOTHROW(AccessPointFactoryLinux{ std::make_unique<Test::AccessPointControllerFactoryTest>() });
    }

    SECTION("Create multiple doesn't cause a crash")
    {
        REQUIRE_NOTHROW(AccessPointFactoryLinux{ std::make_unique<Test::AccessPointControllerFactoryTest>() });
        REQUIRE_NOTHROW(AccessPointFactoryLinux{ std::make_unique<Test::AccessPointControllerFactoryTest>() });
    }
}

TEST_CASE("Destroy an AccessPointFactoryLinux instance", "[wifi][core][ap][linux]")
{
    using namespace Microsoft::Net::Wifi;

    SECTION("Destroy doesn't cause a crashk with null controller factory")
    {
        AccessPointFactoryLinux accessPointFactory{ nullptr };
        REQUIRE_NOTHROW(accessPointFactory.~AccessPointFactoryLinux());
    }

    SECTION("Destroy doesn't cause a crash")
    {
        AccessPointFactoryLinux accessPointFactory{ std::make_unique<Test::AccessPointControllerFactoryTest>() };
        REQUIRE_NOTHROW(accessPointFactory.~AccessPointFactoryLinux());
    }
}

TEST_CASE("Create() function", "[wifi][core][ap][linux]")
{
    using namespace Microsoft::Net::Wifi;

    SECTION("Create() doesn't cause a crash")
    {
        AccessPointFactoryLinux accessPointFactory{ std::make_unique<Test::AccessPointControllerFactoryTest>() };
        REQUIRE_NOTHROW(accessPointFactory.Create(Test::InterfaceNameDefault));
    }

    SECTION("Create() with empty/null inteface doesn't cause a crash")
    {
        AccessPointFactoryLinux accessPointFactory{ std::make_unique<Test::AccessPointControllerFactoryTest>() };
        REQUIRE_NOTHROW(accessPointFactory.Create({}));
    }

    SECTION("Create() with valid input arguments returns non-nullptr instance")
    {
        AccessPointFactoryLinux accessPointFactory{ std::make_unique<Test::AccessPointControllerFactoryTest>() };
        REQUIRE(accessPointFactory.Create(Test::InterfaceNameDefault) != nullptr);
    }

    SECTION("Create() instance reflects basic properties from base class")
    {
        AccessPointFactoryLinux accessPointFactory{ std::make_unique<Test::AccessPointControllerFactoryTest>() };
        const auto accessPoint = accessPointFactory.Create(Test::InterfaceNameDefault);
        REQUIRE(accessPoint->GetInterface() == Test::InterfaceNameDefault);
    }
}
