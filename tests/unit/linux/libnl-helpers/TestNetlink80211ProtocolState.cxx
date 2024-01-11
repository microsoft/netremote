
#include <catch2/catch_test_macros.hpp>
#include <magic_enum.hpp>
#include <microsoft/net/netlink/nl80211/Netlink80211ProtocolState.hxx>

TEST_CASE("Netlink80211ProtocolState instance creation", "[linux][libnl-helpers]")
{
    using namespace Microsoft::Net::Netlink::Nl80211;

    SECTION("Create doesn't cause a crash")
    {
        REQUIRE_NOTHROW(Nl80211ProtocolState::Instance());
    }

    SECTION("Create initializes driver id")
    {
        auto& protocolState{ Nl80211ProtocolState::Instance() };
        REQUIRE(protocolState.DriverId != -1);
    }

    SECTION("Create initializes multicast group ids")
    {
        auto& protocolState{ Nl80211ProtocolState::Instance() };
        REQUIRE(std::size(protocolState.MulticastGroupId) == magic_enum::enum_count<Nl80211MulticastGroup>());

        for (const auto& [_, multicastGroupId] : protocolState.MulticastGroupId) {
            REQUIRE(multicastGroupId != -1);
        }
    }

    SECTION("Instance is a singleton")
    {
        auto& protocolState1{ Nl80211ProtocolState::Instance() };
        auto& protocolState2{ Nl80211ProtocolState::Instance() };
        REQUIRE(&protocolState1 == &protocolState2);
    }
}
