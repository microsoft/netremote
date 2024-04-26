
#include <cstdint>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include <microsoft/net/INetworkOperations.hxx>
#include <microsoft/net/remote/NetRemoteDiscoveryService.hxx>

using namespace Microsoft::Net::Remote;
using Microsoft::Net::INetworkOperations;
using Microsoft::Net::IpInformation;
using Microsoft::Net::Ipv4AnyAddress;

NetRemoteDiscoveryService::NetRemoteDiscoveryService(std::string hostname, uint32_t port, std::unordered_map<std::string, IpInformation> ipAddresses) :
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

const std::unordered_map<std::string, Microsoft::Net::IpInformation>&
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
    using Microsoft::Net::Ipv4AnyAddress;

    std::vector<std::string> ipAddressesAll{};

    // If the "any address" is specified, the server will be bound to all available addresses, so resolve them.
    if (ipAddress.contains(Ipv4AnyAddress)) {
        auto ipAddressesLocal = m_networkOperations->GetLocalIpAddresses();
        ipAddressesAll = { std::make_move_iterator(std::begin(ipAddressesLocal)), std::make_move_iterator(std::end(ipAddressesLocal)) };
    } else {
        ipAddressesAll = { std::move(ipAddress) };
    }

    // Get information about the IP addresses and add them to the map.
    for (auto& ipAddressResolved : ipAddressesAll) {
        auto ipAddressInfo = m_networkOperations->GetLocalIpInformation(ipAddressResolved);
        m_ipAddresses[std::move(ipAddressResolved)] = std::move(ipAddressInfo);
    }

    return *this;
}

std::shared_ptr<NetRemoteDiscoveryService>
NetRemoteDiscoveryServiceBuilder::Build()
{
    return m_discoveryServiceFactory->Create(m_hostname, m_port, m_ipAddresses);
}
