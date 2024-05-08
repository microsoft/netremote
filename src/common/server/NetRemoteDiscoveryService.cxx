
#include <cstdint>
#include <format>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include <microsoft/net/INetworkOperations.hxx>
#include <microsoft/net/IpAddressInformation.hxx>
#include <microsoft/net/remote/NetRemoteDiscoveryService.hxx>

using namespace Microsoft::Net::Remote;
using Microsoft::Net::INetworkOperations;
using Microsoft::Net::IpAddressInformation;

NetRemoteDiscoveryService::NetRemoteDiscoveryService(std::string hostname, uint32_t port, std::unordered_map<std::string, IpAddressInformation> ipAddresses) :
    m_hostname(std::move(hostname)),
    m_port(port),
    m_ipAddresses(std::move(ipAddresses))
{}

std::string_view
NetRemoteDiscoveryService::GetHostname() const noexcept
{
    return m_hostname;
}

uint32_t
NetRemoteDiscoveryService::GetPort() const noexcept
{
    return m_port;
}

const std::unordered_map<std::string, Microsoft::Net::IpAddressInformation>&
NetRemoteDiscoveryService::GetIpAddresses() const noexcept
{
    return m_ipAddresses;
}

NetRemoteDiscoveryServiceBuilder::NetRemoteDiscoveryServiceBuilder(std::unique_ptr<INetRemoteDiscoveryServiceFactory> discoveryServiceFactory, std::unique_ptr<INetworkOperations> networkOperations) :
    m_discoveryServiceFactory(std::move(discoveryServiceFactory)),
    m_networkOperations(std::move(networkOperations))
{}

NetRemoteDiscoveryServiceBuilder&
NetRemoteDiscoveryServiceBuilder::SetHostname(std::string hostname)
{
    m_hostname = std::move(hostname);
    return *this;
}

NetRemoteDiscoveryServiceBuilder&
NetRemoteDiscoveryServiceBuilder::SetPort(uint32_t port)
{
    m_port = port;
    return *this;
}

NetRemoteDiscoveryServiceBuilder&
NetRemoteDiscoveryServiceBuilder::AddIpAddress(std::string ipAddress)
{
    auto ipAddressInfo = m_networkOperations->GetLocalIpAddressInformation(ipAddress);
    if (std::empty(ipAddressInfo)) {
        throw std::invalid_argument(std::format("Invalid IP address: {}", ipAddress));
    }

    m_ipAddresses.merge(ipAddressInfo);

    return *this;
}

std::shared_ptr<NetRemoteDiscoveryService>
NetRemoteDiscoveryServiceBuilder::Build()
{
    return m_discoveryServiceFactory->Create(m_hostname, m_port, m_ipAddresses);
}
