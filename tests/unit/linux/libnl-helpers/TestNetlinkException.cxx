
#include <catch2/catch_test_macros.hpp>
#include <microsoft/net/netlink/NetlinkException.hxx>
#include <netlink/errno.h>

TEST_CASE("NetlinkException", "[linux][libnl-helpers]")
{
    using Microsoft::Net::Netlink::NetlinkException;

    SECTION("Does not crash")
    {
        REQUIRE_THROWS_AS(throw NetlinkException(NLE_BAD_SOCK, "test"), NetlinkException);
    }
}
