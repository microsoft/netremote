
#ifndef NET_REMOTE_DISCOVERY_SERVICE_HXX
#define NET_REMOTE_DISCOVERY_SERVICE_HXX

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include <microsoft/net/INetworkOperations.hxx>
#include <microsoft/net/remote/protocol/NetRemoteProtocol.hxx>

namespace Microsoft::Net::Remote::Service
{
/**
 * @brief Collection of netremote discovery service configuration.
 */
struct NetRemoteDiscoveryServiceConfiguration
{
    uint16_t Port{ Protocol::NetRemoteProtocol::PortDefault };
    std::string Name{ Protocol::NetRemoteProtocol::DnssdServiceName };
    std::string Protocol{ Protocol::NetRemoteProtocol::DnssdServiceProtocol };
    std::unordered_map<std::string, Microsoft::Net::IpAddressInformation> IpAddresses{};
};

/**
 * @brief Network service enabling clients to discover netremote servers on the network.
 */
struct NetRemoteDiscoveryService
{
    /**
     * @brief Construct a new Net RemoteDiscoveryService object with the specified configuration.
     *
     * @param discoveryServiceConfiguration The configuration for the service.
     */
    NetRemoteDiscoveryService(NetRemoteDiscoveryServiceConfiguration discoveryServiceConfiguration);

    virtual ~NetRemoteDiscoveryService() = default;

    NetRemoteDiscoveryService(const NetRemoteDiscoveryService&) = delete;

    NetRemoteDiscoveryService(NetRemoteDiscoveryService&&) = delete;

    NetRemoteDiscoveryService&
    operator=(const NetRemoteDiscoveryService&) = delete;

    NetRemoteDiscoveryService&
    operator=(NetRemoteDiscoveryService&&) = delete;

    /**
     * @brief Start the discovery service. This will begin advertising the service on the network.
     */
    virtual void
    Start() = 0;

    /**
     * @brief Stop the discovery service. This will stop advertising the service on the network, preventing it from
     * being discoverable by clients.
     */
    virtual void
    Stop() = 0;

    /**
     * @brief Get the service name.
     *
     * @return std::string_view
     */
    std::string_view
    GetServiceName() const noexcept;

    /**
     * @brief Get the service protocol type.
     *
     * @return std::string_view
     */
    std::string_view
    GetProtocol() const noexcept;

    /**
     * @brief Get the port of the service.
     *
     * @return uint16_t
     */
    uint16_t
    GetPort() const noexcept;

    /**
     * @brief Get the IP addresses the service listens on.
     *
     * @return const std::unordered_map<std::string, Microsoft::Net::IpAddressInformation>&
     */
    const std::unordered_map<std::string, Microsoft::Net::IpAddressInformation>&
    GetIpAddresses() const noexcept;

private:
    NetRemoteDiscoveryServiceConfiguration m_configuration;
};

/**
 * @brief Factory to create a NetRemoteDiscoveryService.
 *
 * This facilitates multiple implementations of the service, including across operating systems and mock implementations
 * for testing.
 */
struct INetRemoteDiscoveryServiceFactory
{
    virtual ~INetRemoteDiscoveryServiceFactory() = default;

    /**
     * @brief Create a new NetRemoteDiscoveryService object with the specified configuration.
     *
     * @param discoveryServiceConfiguration
     * @return std::shared_ptr<NetRemoteDiscoveryService>
     */
    virtual std::unique_ptr<NetRemoteDiscoveryService>
    Create(NetRemoteDiscoveryServiceConfiguration discoveryServiceConfiguration) = 0;
};
} // namespace Microsoft::Net::Remote::Service

#endif // NET_REMOTE_DISCOVERY_SERVICE_HXX
