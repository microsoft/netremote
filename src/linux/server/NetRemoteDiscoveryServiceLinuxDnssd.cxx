
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

std::shared_ptr<NetRemoteDiscoveryService>
NetRemoteDiscoveryServiceLinuxDnssdFactory::Create(std::string hostname, uint32_t port, std::unordered_map<std::string, Microsoft::Net::IpAddressInformation> ipAddresses)
{
    return std::make_unique<NetRemoteDiscoveryServiceLinuxDnssd>(std::move(hostname), port, std::move(ipAddresses));
}
