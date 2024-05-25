
#include <ranges>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

#include <microsoft/net/IpAddressInformation.hxx>
#include <microsoft/net/NetworkInterface.hxx>
#include <microsoft/net/NetworkIpAddress.hxx>
#include <microsoft/net/test/NetworkOperationsTest.hxx>

using namespace Microsoft::Net;
using namespace Microsoft::Net::Test;

// clang-format off
NetworkOperationsTest::NetworkOperationsTest() :
    m_networkInterfaceInformation{
        {
            NetworkInterfaceId{
                .Name = "brgateway0",
                .Type = NetworkInterfaceType::Bridge,
            },
            {
                NetworkIpAddress{
                    .Family = NetworkIpFamily::Ipv4,
                    .Address = "8.8.8.8",
                    .PrefixLength = 24,
                },
                NetworkIpAddress{
                    .Family = NetworkIpFamily::Ipv6,
                    .Address = "2001:db8::4",
                    .PrefixLength = 64,
                },
                NetworkIpAddress{
                    .Family = NetworkIpFamily::Ipv6,
                    .Address = "2001:db8::5",
                    .PrefixLength = 64,
                }
            }
        },
        { 
            NetworkInterfaceId{ 
                .Name = "eth10", 
                .Type = NetworkInterfaceType::Ethernet,
            }, 
            {
                NetworkIpAddress{
                    .Family = NetworkIpFamily::Ipv4,
                    .Address = "1.2.3.4",
                    .PrefixLength = 24,
                },
                NetworkIpAddress{
                    .Family = NetworkIpFamily::Ipv6,
                    .Address = "2001:db8::1",
                    .PrefixLength = 64,
                },
            }
        },
        {
            NetworkInterfaceId{
                .Name = "wlan5",
                .Type = NetworkInterfaceType::Wifi,
            },
            {
                NetworkIpAddress{
                    .Family = NetworkIpFamily::Ipv4,
                    .Address = "192.168.0.1",
                    .PrefixLength = 8,
                },
                NetworkIpAddress{
                    .Family = NetworkIpFamily::Ipv6,
                    .Address = "2001:db8::2",
                    .PrefixLength = 64,
                },
                NetworkIpAddress{
                    .Family = NetworkIpFamily::Ipv6,
                    .Address = "2001:db8::3",
                    .PrefixLength = 64,
                }
            }
        },
        {
            NetworkInterfaceId{
                .Name = "tun0",
                .Type = NetworkInterfaceType::Other,
            },
            {
                NetworkIpAddress{
                    .Family = NetworkIpFamily::Ipv4,
                    .Address = "192.193.194.195",
                    .PrefixLength = 32,
                },
            },
        },
    }
{}
// clang-format on

std::unordered_set<NetworkInterfaceId>
NetworkOperationsTest::EnumerateNetworkInterfaces() const noexcept
{
    // Note: the below could be done more efficiently (with contantants), but this is a test implementation and intends
    // to simulate the minimal processing time it would take to prepare such a list in a real scenario.

    std::unordered_set<NetworkInterfaceId> networkInterfaces{};
    for (auto networkInterfaceId : m_networkInterfaceInformation | std::ranges::views::keys) {
        networkInterfaces.insert(std::move(networkInterfaceId));
    }

    return networkInterfaces;
}

std::unordered_set<NetworkIpAddress>
NetworkOperationsTest::GetNetworkInterfaceAddresses([[maybe_unused]] std::string_view networkInterfaceName) const noexcept
{
    const auto matchesNetworkInterfaceName = [&networkInterfaceName](auto &&networkInterfaceInformation) {
        const auto &[networkInterfaceId, networkInterfaceAddresses] = networkInterfaceInformation;
        return networkInterfaceId.Name == networkInterfaceName;
    };

    auto networkInterfaceId = std::ranges::find_if(m_networkInterfaceInformation, matchesNetworkInterfaceName);
    if (networkInterfaceId == std::cend(m_networkInterfaceInformation)) {
        return {};
    }

    auto [_, networkInterfaceAddresses] = *networkInterfaceId;
    return networkInterfaceAddresses;
}

std::unordered_map<std::string, IpAddressInformation>
NetworkOperationsTest::GetLocalIpAddressInformation([[maybe_unused]] std::string_view ipAddress) const noexcept
{
    return {};
}
