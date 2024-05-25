
#include <iterator>
#include <ranges>
#include <unordered_set>
#include <vector>

#include <microsoft/net/NetworkInterface.hxx>
#include <microsoft/net/NetworkIpAddress.hxx>
#include <microsoft/net/ServiceApiNetworkAdapters.hxx>
#include <microsoft/net/remote/protocol/NetworkCore.pb.h>

namespace Microsoft::Net
{
using Microsoft::Net::NetworkInterface;
using Microsoft::Net::NetworkInterfaceId;

NetworkAddress
ToServiceNetworkAddress(const NetworkIpAddress& networkIpAddress) noexcept
{
    NetworkAddress networkAddress{};
    networkAddress.set_family(ToServiceNetworkAddressFamily(networkIpAddress.Family));
    networkAddress.set_address(networkIpAddress.Address);
    networkAddress.set_prefixlength(networkIpAddress.PrefixLength);

    return networkAddress;
}

NetworkIpAddress
FromServiceNetworkAddress(const NetworkAddress& networkAddress) noexcept
{
    NetworkIpAddress networkIpAddress{
        .Family = FromServiceNetworkAddressFamily(networkAddress.family()),
        .Address = networkAddress.address(),
        .PrefixLength = networkAddress.prefixlength()
    };

    return networkIpAddress;
}

using Microsoft::Net::NetworkAddress;
using Microsoft::Net::NetworkIpAddress;

NetworkInterface
ToServiceNetworkInterface(const NetworkInterfaceId& networkInterfaceId, const std::unordered_set<NetworkIpAddress> networkIpAddresses) noexcept
{
    NetworkInterface networkInterface;
    networkInterface.set_id(networkInterfaceId.Name);
    networkInterface.set_kind(ToServiceNetworkInterfaceKind(networkInterfaceId.Type));

    std::vector<NetworkAddress> networkAddresses{ std::size(networkIpAddresses) };
    std::ranges::transform(networkIpAddresses, std::begin(networkAddresses), ToServiceNetworkAddress);
    *networkInterface.mutable_addresses() = { std::make_move_iterator(std::begin(networkAddresses)), std::make_move_iterator(std::end(networkAddresses)) };

    return networkInterface;
}

NetworkInterfaceId
FromServiceNetworkInterface(const NetworkInterface& networkInterface) noexcept
{
    NetworkInterfaceId networkInterfaceId{
        .Name = networkInterface.id(),
        .Type = FromServiceNetworkInterfaceKind(networkInterface.kind())
    };

    return networkInterfaceId;
}
} // namespace Microsoft::Net
