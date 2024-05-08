
#ifndef NET_REMOTE_DISCOVERY_SERVICE_LINUX_DNSSD_HXX
#define NET_REMOTE_DISCOVERY_SERVICE_LINUX_DNSSD_HXX

#include <memory>
#include <string>
#include <unordered_map>

#include <microsoft/net/INetworkOperations.hxx>
#include <microsoft/net/remote/NetRemoteDiscoveryService.hxx>

namespace Microsoft::Net::Remote
{
/**
 * @brief Linux DNS-SD based service discovery implementation.
 */
struct NetRemoteDiscoveryServiceLinuxDnssd :
    public NetRemoteDiscoveryService
{
    using NetRemoteDiscoveryService::NetRemoteDiscoveryService;

    ~NetRemoteDiscoveryServiceLinuxDnssd() override = default;

    /**
     * Prevent copying and moving of this object.
     */
    NetRemoteDiscoveryServiceLinuxDnssd(const NetRemoteDiscoveryServiceLinuxDnssd&) = delete;

    NetRemoteDiscoveryServiceLinuxDnssd(NetRemoteDiscoveryServiceLinuxDnssd&&) = delete;

    NetRemoteDiscoveryServiceLinuxDnssd&
    operator=(const NetRemoteDiscoveryServiceLinuxDnssd&) = delete;

    NetRemoteDiscoveryServiceLinuxDnssd&
    operator=(NetRemoteDiscoveryServiceLinuxDnssd&&) = delete;

    /**
     * @brief Start the discovery service.
     */
    void
    Start() override;

    /**
     * @brief Stop the discovery service.
     */
    void
    Stop() override;
};

/**
 * @brief Factory to create Linux DNS-SD based discovery service. Creates an instance of NetRemoteDiscoveryServiceLinuxDnssd.
 */
struct NetRemoteDiscoveryServiceLinuxDnssdFactory :
    public INetRemoteDiscoveryServiceFactory
{
    /**
     * @brief Destroy the NetRemoteDiscoveryServiceLinuxDnssdFactory object.
     */
    ~NetRemoteDiscoveryServiceLinuxDnssdFactory() override = default;

    /**
     * @brief Construct a new NetRemoteDiscoveryServiceLinuxDnssdFactory object.
     */
    NetRemoteDiscoveryServiceLinuxDnssdFactory() = default;

    /**
     * Prevent copying and moving of this object.
     */
    NetRemoteDiscoveryServiceLinuxDnssdFactory(const NetRemoteDiscoveryServiceLinuxDnssdFactory&) = delete;

    NetRemoteDiscoveryServiceLinuxDnssdFactory(NetRemoteDiscoveryServiceLinuxDnssdFactory&&) = delete;

    NetRemoteDiscoveryServiceLinuxDnssdFactory&
    operator=(const NetRemoteDiscoveryServiceLinuxDnssdFactory&) = delete;

    NetRemoteDiscoveryServiceLinuxDnssdFactory&
    operator=(NetRemoteDiscoveryServiceLinuxDnssdFactory&&) = delete;

    /**
     * @brief Create a new instance of a NetRemoteDiscoveryServiceLinuxDnssd.
     *
     * @param hostname The hostname of the service.
     * @param port The port the service runs on.
     * @param ipAddresses The IP addresses of the service.
     * @return std::shared_ptr<NetRemoteDiscoveryService>
     */
    std::shared_ptr<NetRemoteDiscoveryService>
    Create(std::string hostname, uint32_t port, std::unordered_map<std::string, Microsoft::Net::IpAddressInformation> ipAddresses) override;
};
} // namespace Microsoft::Net::Remote

#endif // NET_REMOTE_DISCOVERY_SERVICE_LINUX_DNSSD_HXX
