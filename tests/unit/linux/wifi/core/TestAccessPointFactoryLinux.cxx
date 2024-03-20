
#include <memory>
#include <optional>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <microsoft/net/netlink/nl80211/Netlink80211Interface.hxx>
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

    SECTION("Destroy doesn't cause a crash with null controller factory")
    {
        AccessPointFactoryLinux accessPointFactory{ nullptr };
        REQUIRE_NOTHROW(accessPointFactory.~AccessPointFactoryLinux());
    }

    SECTION("Destroy doesn't cause a crash")
    {
        std::optional<AccessPointFactoryLinux> accessPointFactory{};
        accessPointFactory.emplace(std::make_unique<Test::AccessPointControllerFactoryTest>());
        REQUIRE_NOTHROW(accessPointFactory.reset());
    }
}

TEST_CASE("Create() function", "[wifi][core][ap][linux]")
{
    using namespace Microsoft::Net::Wifi;

    using Microsoft::Net::Netlink::Nl80211::Nl80211Interface;

    SECTION("Create() doesn't cause a crash")
    {
        auto createArgs = std::make_unique<AccessPointCreateArgsLinux>(Nl80211Interface{});
        AccessPointFactoryLinux accessPointFactory{ std::make_unique<Test::AccessPointControllerFactoryTest>() };
        REQUIRE_NOTHROW(accessPointFactory.Create(Test::InterfaceNameDefault, std::move(createArgs)));
    }

    SECTION("Create() with empty/null inteface doesn't cause a crash")
    {
        auto createArgs = std::make_unique<AccessPointCreateArgsLinux>(Nl80211Interface{});
        AccessPointFactoryLinux accessPointFactory{ std::make_unique<Test::AccessPointControllerFactoryTest>() };
        REQUIRE_NOTHROW(accessPointFactory.Create({}, std::move(createArgs)));
    }

    SECTION("Create() with valid input arguments returns non-nullptr instance")
    {
        auto createArgs = std::make_unique<AccessPointCreateArgsLinux>(Nl80211Interface{});
        AccessPointFactoryLinux accessPointFactory{ std::make_unique<Test::AccessPointControllerFactoryTest>() };
        REQUIRE(accessPointFactory.Create(Test::InterfaceNameDefault, std::move(createArgs)) != nullptr);
    }

    SECTION("Create() instance reflects basic properties from base class")
    {
        auto createArgs = std::make_unique<AccessPointCreateArgsLinux>(Nl80211Interface{});
        AccessPointFactoryLinux accessPointFactory{ std::make_unique<Test::AccessPointControllerFactoryTest>() };
        const auto accessPoint = accessPointFactory.Create(Test::InterfaceNameDefault, std::move(createArgs));
        REQUIRE(accessPoint->GetInterfaceName() == Test::InterfaceNameDefault);
    }
}
