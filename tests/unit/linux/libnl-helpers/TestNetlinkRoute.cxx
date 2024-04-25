
#include <catch2/catch_test_macros.hpp>
#include <microsoft/net/netlink/route/NetlinkRoute.hxx>

TEST_CASE("NetlinkEnumerateIpv4Addresses", "[linux][libnl-helpers]")
{
    using Microsoft::Net::Netlink::NetlinkEnumerateIpv4Addresses;

    SECTION("Doesn't cause a crash")
    {
        REQUIRE_NOTHROW(NetlinkEnumerateIpv4Addresses());
    }

    SECTION("Returned values are non-empty")
    {
        const auto addresses = NetlinkEnumerateIpv4Addresses();

        for (const auto& address : addresses) {
            REQUIRE_FALSE(std::empty(address));
        }
    }
}

TEST_CASE("NetlinkEnumerateInterfaceInfo", "[linux][libnl-helpers]")
{
    using Microsoft::Net::Netlink::NetlinkEnumerateInterfaceInfo;

    SECTION("Doesn't cause a crash")
    {
        REQUIRE_NOTHROW(NetlinkEnumerateInterfaceInfo());
    }
}
