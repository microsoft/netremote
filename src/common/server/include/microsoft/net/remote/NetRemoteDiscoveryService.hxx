
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

namespace Microsoft::Net::Remote
{
/**
 * @brief Collection of netremote discovery service configuration.
 */
struct NetRemoteDiscoveryServiceConfiguration
{
    uint16_t Port{ Protocol::NetRemoteProtocol::PortDefault };
    std::string Name{ Protocol::NetRemoteProtocol::DnssdServiceName };
    std::string Protocol{ Protocol::NetRemoteProtocol::DnssdServiceProtocol };
    std::string Hostname;
    std::unordered_map<std::string, Microsoft::Net::IpAddressInformation> IpAddresses;
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

protected:
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
     * @brief Get the host name of the service.
     *
     * @return std::string_view
     */
    std::string_view
    GetHostname() const noexcept;

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

/**
 * @brief Helper to build a NetRemoteDiscoveryService.
 *
 * Note that this is not thread-safe.
 */
struct NetRemoteDiscoveryServiceBuilder
{
    /**
     * @brief Construct a new NetRemoteDiscoveryServiceBuilder object.
     *
     * @param discoveryServiceFactory The factory to use to create the service instance.
     * @param networkOperations The object to use when performing network operations.
     */
    NetRemoteDiscoveryServiceBuilder(std::unique_ptr<INetRemoteDiscoveryServiceFactory> discoveryServiceFactory, std::unique_ptr<INetworkOperations> networkOperations);

    /**
     * @brief Destroy the NetRemoteDiscoveryServiceBuilder object.
     */
    virtual ~NetRemoteDiscoveryServiceBuilder() = default;

    /**
     * @brief Set the service name.
     *
     * @param serviceName The name of the service to set.
     * @return NetRemoteDiscoveryServiceBuilder&
     */
    NetRemoteDiscoveryServiceBuilder&
    SetServiceName(std::string serviceName);

    /**
     * @brief Set the service protocol type.
     *
     * @param serviceProtocol The protocol type to set.
     * @return NetRemoteDiscoveryServiceBuilder&
     */
    NetRemoteDiscoveryServiceBuilder&
    SetServiceProtocol(std::string serviceProtocol);

    /**
     * @brief Set the hostname of the service.
     *
     * @param hostname The hostname to set.
     * @return NetRemoteDiscoveryServiceBuilder&
     */
    NetRemoteDiscoveryServiceBuilder&
    SetHostname(std::string hostname);

    /**
     * @brief Set the port of the service.
     *
     * If not specified, the default port is used.
     *
     * @param port The port the service listens on.
     * @return NetRemoteDiscoveryServiceBuilder&
     */
    NetRemoteDiscoveryServiceBuilder&
    SetPort(uint16_t port);

    /**
     * @brief Add an IP address the service listens on.
     *
     * @param ipAddress The ip address the service listens on.
     * @return NetRemoteDiscoveryServiceBuilder&
     */
    NetRemoteDiscoveryServiceBuilder&
    AddIpAddress(std::string ipAddress);

    /**
     * @brief Create a NetRemoteDiscoveryService object with the stored configuration.
     *
     * @return std::shared_ptr<NetRemoteDiscoveryService>
     */
    std::shared_ptr<NetRemoteDiscoveryService>
    Build();

private:
    std::unique_ptr<INetRemoteDiscoveryServiceFactory> m_discoveryServiceFactory;
    std::unique_ptr<Microsoft::Net::INetworkOperations> m_networkOperations;
    NetRemoteDiscoveryServiceConfiguration m_discoveryServiceConfiguration;
};
} // namespace Microsoft::Net::Remote

#endif // NET_REMOTE_DISCOVERY_SERVICE_HXX
