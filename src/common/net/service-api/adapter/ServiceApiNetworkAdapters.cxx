
#include <microsoft/net/NetworkInterface.hxx>
#include <microsoft/net/NetworkIpAddress.hxx>
#include <microsoft/net/ServiceApiNetworkAdapters.hxx>
#include <microsoft/net/remote/protocol/NetworkCore.pb.h>

namespace Microsoft::Net
{
using Microsoft::Net::NetworkInterfaceId;

Microsoft::Net::NetworkAddress
ToServiceNetworkAddress([[maybe_unused]] const Microsoft::Net::NetworkIpAddress& networkIpAddress) noexcept
{
    // TODO
    return {};
}

Microsoft::Net::NetworkIpAddress
FromServiceNetworkAddress([[maybe_unused]] const Microsoft::Net::NetworkAddress& networkAddress) noexcept
{
    // TODO
    return {};
}

Microsoft::Net::NetworkInterface
ToServiceNetworkInterface([[maybe_unused]] const Microsoft::Net::NetworkInterfaceId& networkInterfaceId) noexcept
{
    // TODO
    return {};
}

Microsoft::Net::NetworkInterfaceId
FromServiceNetworkInterface([[maybe_unused]] const Microsoft::Net::NetworkInterface& networkInterface) noexcept
{
    // TODO
    return {};
}
} // namespace Microsoft::Net
