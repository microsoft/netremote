
#include <catch2/catch_test_macros.hpp>
#include <microsoft/net/netlink/nl80211/Netlink80211Interface.hxx>

TEST_CASE("Nl80211Interface instance creation", "[linux][libnl-helpers]")
{
    using Microsoft::Net::Netlink::Nl80211::Nl80211Interface;

    SECTION("Parse doesn't cause a crash with null input")
    {
        struct nl_msg *nl80211Message{ nullptr };
        REQUIRE_NOTHROW(Nl80211Interface::Parse(nl80211Message));
    }

    SECTION("Enumerate doesn't cause a crash")
    {
        REQUIRE_NOTHROW(Nl80211Interface::Enumerate());
    }
}
