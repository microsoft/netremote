
#ifndef NET_REMOTE_DISCOVERY_SERVICE_LINUX_DNSSD_HXX
#define NET_REMOTE_DISCOVERY_SERVICE_LINUX_DNSSD_HXX

#include <memory>

#include <microsoft/net/remote/NetRemoteDiscoveryService.hxx>

namespace Microsoft::Net::Remote
{
struct NetRemoteDiscoveryServiceLinuxDnssd :
    public NetRemoteDiscoveryService
{
    using NetRemoteDiscoveryService::NetRemoteDiscoveryService;

    ~NetRemoteDiscoveryServiceLinuxDnssd() override = default;

    NetRemoteDiscoveryServiceLinuxDnssd(const NetRemoteDiscoveryServiceLinuxDnssd&) = delete;

    NetRemoteDiscoveryServiceLinuxDnssd(NetRemoteDiscoveryServiceLinuxDnssd&&) = delete;

    NetRemoteDiscoveryServiceLinuxDnssd&
    operator=(const NetRemoteDiscoveryServiceLinuxDnssd&) = delete;

    NetRemoteDiscoveryServiceLinuxDnssd&
    operator=(NetRemoteDiscoveryServiceLinuxDnssd&&) = delete;

    void
    Start() override;

    void
    Stop() override;
};
} // namespace Microsoft::Net::Remote

#endif // NET_REMOTE_DISCOVERY_SERVICE_LINUX_DNSSD_HXX
