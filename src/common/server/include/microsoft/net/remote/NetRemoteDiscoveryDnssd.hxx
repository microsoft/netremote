
#ifndef NET_REMOTE_DISCOVERY_DNSSD_HXX
#define NET_REMOTE_DISCOVERY_DNSSD_HXX

#include <microsoft/net/remote/NetRemoteDiscoveryService.hxx>

namespace Microsoft::Net::Remote
{
struct DnssdServiceBuilder
{
    static constexpr auto NetRemoteDnssdServiceName = "netremote";

    /**
     * @brief Note that all non-TCP protocols must use "udp" as the protocol value, even if the protocol is not "udp".
     * See RFC 6763 "DNS-Based Service Discovery", section 4.1.2 "Service Names" and and section 7 "Service Names" for
     * additional details and reasoning.
     */
    static constexpr auto NetRemoteDnssdServiceProtocol = "udp";
};
} // namespace Microsoft::Net::Remote

#endif // NET_REMOTE_DISCOVERY_DNSSD_HXX
