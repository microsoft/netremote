
#include <optional>

#include <catch2/catch_test_macros.hpp>
#include <Wpa/Hostapd.hxx>

#include "detail/WpaDaemonManager.hxx"

TEST_CASE("Create a Hostapd", "[wpa][hostapd][client][remote]")
{
    using namespace Wpa;

    SECTION("Create doesn't cause a crash")
    {
        std::optional<Hostapd> hostapd;
        REQUIRE_NOTHROW(hostapd.emplace(WpaDaemonManager::InterfaceNameDefault));
    }
}
