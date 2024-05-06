
#include <format>
#include <iostream>

#include <catch2/catch_test_macros.hpp>
#include <microsoft/net/NetworkOperationsLinux.hxx>

TEST_CASE("GetLocalIpAddressInformation IPv4 Addresses", "[linux][server]")
{
    using namespace Microsoft::Net;

    constexpr auto Ipv4FixedWithoutPort{ "127.0.0.1" };
    constexpr auto Ipv4FixedWithPort{ "127.0.0.1:1234" };
    constexpr auto Ipv4AnyAddressWithoutPort{ "0.0.0.0" };
    constexpr auto Ipv4AnyAddressWithPort{ "0.0.0.0:1234" };

    SECTION("Ipv4 fixed address without port doesn't cause a crash")
    {
        REQUIRE_NOTHROW(NetworkOperationsLinux{}.GetLocalIpAddressInformation(Ipv4FixedWithoutPort));
    }

    SECTION("Ipv4 fixed address with port doesn't cause a crash")
    {
        REQUIRE_NOTHROW(NetworkOperationsLinux{}.GetLocalIpAddressInformation(Ipv4FixedWithPort));
    }

    SECTION("Ipv4 any address without port doesn't cause a crash")
    {
        REQUIRE_NOTHROW(NetworkOperationsLinux{}.GetLocalIpAddressInformation(Ipv4AnyAddressWithoutPort));
    }

    SECTION("Ipv4 any address with port doesn't cause a crash")
    {
        REQUIRE_NOTHROW(NetworkOperationsLinux{}.GetLocalIpAddressInformation(Ipv4AnyAddressWithPort));
    }

    SECTION("Ipv4 any address without port resolves all addresses")
    {
        const auto ipInfo = NetworkOperationsLinux{}.GetLocalIpAddressInformation(Ipv4AnyAddressWithoutPort);
        REQUIRE(!std::empty(ipInfo));
    }
}

TEST_CASE("GetLocalIpAddressInformation IPv6 Addresses", "[linux][server]")
{
    using namespace Microsoft::Net;

    constexpr auto Ipv6FixedWithoutPort{ "::1" };
    constexpr auto Ipv6FixedWithPort{ "[::1]:1234" };
    constexpr auto Ipv6AnyNoBracketsNoPort{ "::" };
    constexpr auto Ipv6AnyNoBracketsWithPort{ "::1234" };
    constexpr auto Ipv6AnyBracketsNoPort{ "[::]" };
    constexpr auto Ipv6AnyBracketsWithPort{ "[::]:1234" };

    SECTION("Ipv6 fixed address without port doesn't cause a crash")
    {
        REQUIRE_NOTHROW(NetworkOperationsLinux{}.GetLocalIpAddressInformation(Ipv6FixedWithoutPort));
    }

    SECTION("Ipv6 fixed address with port doesn't cause a crash")
    {
        REQUIRE_NOTHROW(NetworkOperationsLinux{}.GetLocalIpAddressInformation(Ipv6FixedWithPort));
    }

    SECTION("Ipv6 any address without brackets and no port doesn't cause a crash")
    {
        REQUIRE_NOTHROW(NetworkOperationsLinux{}.GetLocalIpAddressInformation(Ipv6AnyNoBracketsNoPort));
    }

    SECTION("Ipv6 any address without brackets and with port doesn't cause a crash")
    {
        REQUIRE_NOTHROW(NetworkOperationsLinux{}.GetLocalIpAddressInformation(Ipv6AnyNoBracketsWithPort));
    }

    SECTION("Ipv6 any address with brackets and no port doesn't cause a crash")
    {
        REQUIRE_NOTHROW(NetworkOperationsLinux{}.GetLocalIpAddressInformation(Ipv6AnyBracketsNoPort));
    }

    SECTION("Ipv6 any address with brackets and with port doesn't cause a crash")
    {
        REQUIRE_NOTHROW(NetworkOperationsLinux{}.GetLocalIpAddressInformation(Ipv6AnyBracketsWithPort));
    }

    SECTION("Ipv6 any address without brackets and no port resolves all addresses")
    {
        const auto ipInfo = NetworkOperationsLinux{}.GetLocalIpAddressInformation(Ipv6AnyNoBracketsNoPort);
        REQUIRE(!std::empty(ipInfo));
    }
}
