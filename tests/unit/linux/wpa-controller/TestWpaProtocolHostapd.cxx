
#include <Wpa/ProtocolHostapd.hxx>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Test WpaKeyManagement conversions", "[wpa][hostapd][client]")
{
    using namespace Wpa;

    SECTION("Conversion is stable")
    {
        for (auto wpaKeyManagement : AllWpaKeyManagementsValid) {
            REQUIRE(WpaKeyManagementFromPropertyValue(WpaKeyManagementPropertyValue(wpaKeyManagement)) == wpaKeyManagement);
        }
    }
}
