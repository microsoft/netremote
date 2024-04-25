
#ifndef NET_REMOTE_DISCOVERY_DNSSD_HXX
#define NET_REMOTE_DISCOVERY_DNSSD_HXX

#include <microsoft/net/remote/NetRemoteDiscoveryService.hxx>

namespace Microsoft::Net::Remote
{
struct DnssdServiceBuilder
{
    static constexpr auto NetRemoteDnssdServiceName = "netremote";
    static constexpr auto NetRemoteDnssdServiceProtocol = "udp";
};
} // namespace Microsoft::Net::Remote

#endif // NET_REMOTE_DISCOVERY_DNSSD_HXX
