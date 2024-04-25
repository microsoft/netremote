
#ifndef NET_REMOTE_DISCOVERY_DNSSD_HXX
#define NET_REMOTE_DISCOVERY_DNSSD_HXX

#include <string>

#include <microsoft/net/remote/NetRemoteDiscoveryService.hxx>

namespace Microsoft::Net::Remote
{
std::string
BuildDnssdTxtRecord();
} // namespace Microsoft::Net::Remote

#endif // NET_REMOTE_DISCOVERY_DNSSD_HXX
