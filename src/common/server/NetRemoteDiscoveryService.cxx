
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

NetRemoteDiscoveryService::NetRemoteDiscoveryService(NetRemoteDiscoveryServiceConfiguration discoveryServiceConfiguration) :
    m_configuration(std::move(discoveryServiceConfiguration))
{}

std::string_view
NetRemoteDiscoveryService::GetServiceName() const noexcept
{
    return m_configuration.Name;
}

std::string_view
NetRemoteDiscoveryService::GetProtocol() const noexcept
{
    return m_configuration.Protocol;
}

std::string_view
NetRemoteDiscoveryService::GetHostname() const noexcept
{
    return m_configuration.Hostname;
}

uint16_t
NetRemoteDiscoveryService::GetPort() const noexcept
{
    return m_configuration.Port;
}

const std::unordered_map<std::string, Microsoft::Net::IpAddressInformation>&
NetRemoteDiscoveryService::GetIpAddresses() const noexcept
{
    return m_configuration.IpAddresses;
}

NetRemoteDiscoveryServiceBuilder::NetRemoteDiscoveryServiceBuilder(std::unique_ptr<INetRemoteDiscoveryServiceFactory> discoveryServiceFactory, std::unique_ptr<INetworkOperations> networkOperations) :
    m_discoveryServiceFactory(std::move(discoveryServiceFactory)),
    m_networkOperations(std::move(networkOperations))
{}

NetRemoteDiscoveryServiceBuilder&
NetRemoteDiscoveryServiceBuilder::SetServiceName(std::string serviceName)
{
    m_discoveryServiceConfiguration.Name = std::move(serviceName);
    return *this;
}

NetRemoteDiscoveryServiceBuilder&
NetRemoteDiscoveryServiceBuilder::SetServiceProtocol(std::string serviceProtocol)
{
    m_discoveryServiceConfiguration.Protocol = std::move(serviceProtocol);
    return *this;
}

NetRemoteDiscoveryServiceBuilder&
NetRemoteDiscoveryServiceBuilder::SetHostname(std::string hostname)
{
    m_discoveryServiceConfiguration.Hostname = std::move(hostname);
    return *this;
}

NetRemoteDiscoveryServiceBuilder&
NetRemoteDiscoveryServiceBuilder::SetPort(uint16_t port)
{
    m_discoveryServiceConfiguration.Port = port;
    return *this;
}

NetRemoteDiscoveryServiceBuilder&
NetRemoteDiscoveryServiceBuilder::AddIpAddress(std::string ipAddress)
{
    auto ipAddressInfo = m_networkOperations->GetLocalIpAddressInformation(ipAddress);
    if (std::empty(ipAddressInfo)) {
        throw std::invalid_argument(std::format("Invalid IP address: {}", ipAddress));
    }

    m_discoveryServiceConfiguration.IpAddresses.merge(ipAddressInfo);
    return *this;
}

std::shared_ptr<NetRemoteDiscoveryService>
NetRemoteDiscoveryServiceBuilder::Build()
{
    return m_discoveryServiceFactory->Create(std::move(m_discoveryServiceConfiguration));
}
