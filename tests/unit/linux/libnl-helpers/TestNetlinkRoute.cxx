
#include <catch2/catch_test_macros.hpp>
#include <microsoft/net/netlink/route/NetlinkRoute.hxx>

TEST_CASE("NetlinkEnumerateIpv4Addresses", "[linux][libnl-helpers]")
{
    using Microsoft::Net::Netlink::NetlinkEnumerateIpv4Addresses;

    SECTION("Doesn't cause a crash")
    {
        REQUIRE_NOTHROW(NetlinkEnumerateIpv4Addresses());
    }

    SECTION("If value is returned, it is non-empty")
    {
        const auto addresses = NetlinkEnumerateIpv4Addresses();
        if (addresses.has_value()) {
            REQUIRE_FALSE(std::empty(addresses.value()));
        }
    }

    SECTION("If values are returned, they are non-empty")
    {
        const auto addresses = NetlinkEnumerateIpv4Addresses();
        if (addresses.has_value()) {
            for (const auto& address : addresses.value()) {
                REQUIRE_FALSE(std::empty(address));
            }
        }
    }
}
