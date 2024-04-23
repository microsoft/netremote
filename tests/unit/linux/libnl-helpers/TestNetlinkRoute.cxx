
#include <catch2/catch_test_macros.hpp>
#include <microsoft/net/netlink/route/NetlinkRoute.hxx>

TEST_CASE("NetlinkEnumerateIpv4Addresses", "[linux][libnl-helpers]")
{
    using Microsoft::Net::Netlink::NetlinkEnumerateIpv4Addresses2;

    SECTION("Doesn't cause a crash")
    {
        REQUIRE_NOTHROW(NetlinkEnumerateIpv4Addresses2());
    }
}
