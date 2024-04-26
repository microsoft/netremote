
#ifndef NET_REMOTE_DISCOVERY_SERVICE_HXX
#define NET_REMOTE_DISCOVERY_SERVICE_HXX

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

#include <microsoft/net/INetworkOperations.hxx>
#include <microsoft/net/remote/protocol/NetRemoteProtocol.hxx>

namespace Microsoft::Net::Remote
{
struct NetRemoteDiscoveryService
{
    NetRemoteDiscoveryService(std::string hostname, uint32_t port, std::unordered_map<std::string, Microsoft::Net::IpInformation> ipAddresses);

    virtual ~NetRemoteDiscoveryService() = default;

    NetRemoteDiscoveryService(const NetRemoteDiscoveryService&) = delete;

    NetRemoteDiscoveryService(NetRemoteDiscoveryService&&) = delete;

    NetRemoteDiscoveryService&
    operator=(const NetRemoteDiscoveryService&) = delete;

    NetRemoteDiscoveryService&
    operator=(NetRemoteDiscoveryService&&) = delete;

    virtual void
    Start() = 0;

    virtual void
    Stop() = 0;

protected:
    std::string_view
    GetHostname() const noexcept;

    uint32_t
    GetPort() const noexcept;

    const std::unordered_map<std::string, Microsoft::Net::IpInformation>&
    GetIpAddresses() const noexcept;

private:
    std::string m_hostname;
    uint32_t m_port;
    std::unordered_map<std::string, Microsoft::Net::IpInformation> m_ipAddresses;
};

struct INetRemoteDiscoveryServiceFactory
{
    virtual ~INetRemoteDiscoveryServiceFactory() = default;

    virtual std::shared_ptr<NetRemoteDiscoveryService>
    Create(std::string hostname, uint32_t port, std::unordered_map<std::string, Microsoft::Net::IpInformation> ipAddresses) = 0;
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
    SetPort(uint32_t port);

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
    std::string m_hostname;
    uint32_t m_port{ Protocol::NetRemoteProtocol::PortDefault };
    std::unordered_map<std::string, Microsoft::Net::IpInformation> m_ipAddresses{};
};
} // namespace Microsoft::Net::Remote

#endif // NET_REMOTE_DISCOVERY_SERVICE_HXX
