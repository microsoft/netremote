
#include "NetRemoteDiscoveryServiceLinuxDnssd.hxx"

using namespace Microsoft::Net::Remote;

void
NetRemoteDiscoveryServiceLinuxDnssd::Start()
{
    // TODO
}

void
NetRemoteDiscoveryServiceLinuxDnssd::Stop()
{
    // TODO
}

std::unique_ptr<NetRemoteDiscoveryService>
NetRemoteDiscoveryServiceLinuxDnssdFactory::Create(NetRemoteDiscoveryServiceConfiguration discoveryServiceConfiguration)
{
    return std::make_unique<NetRemoteDiscoveryServiceLinuxDnssd>(std::move(discoveryServiceConfiguration));
}
