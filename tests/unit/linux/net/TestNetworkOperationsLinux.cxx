
#include <format>
#include <iostream>

#include <catch2/catch_test_macros.hpp>
#include <microsoft/net/NetworkOperationsLinux.hxx>

TEST_CASE("GetLocalIpAddressInformation", "[linux][server]")
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
        for (const auto& [ip, info] : ipInfo)
        {
            std::cout << std::format("IP: {}, Family: {}, InterfaceType: {}\n", ip, static_cast<int>(info.Family), static_cast<int>(info.InterfaceType));
        }
    }
}
