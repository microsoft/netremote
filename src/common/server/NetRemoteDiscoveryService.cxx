
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

#include <microsoft/net/remote/NetRemoteDiscoveryService.hxx>

using namespace Microsoft::Net::Remote;

namespace detail
{
constexpr auto Ipv4AnyAddress{ "0.0.0.0" };
} // namespace detail

NetRemoteDiscoveryService::NetRemoteDiscoveryService(std::string hostname, uint32_t port, std::unordered_map<std::string, Microsoft::Net::IpInformation> ipAddresses) :
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

NetRemoteDiscoveryServiceBuilder::NetRemoteDiscoveryServiceBuilder(std::unique_ptr<INetRemoteRemoteDiscoveryServiceFactory> discoveryServiceFactory, std::unique_ptr<INetworkOperations> networkOperations) :
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
    // If the "any address" is specified, the server will be bound to all available addresses, so resolve them.
    if (ipAddress.contains(detail::Ipv4AnyAddress)) {
        auto ipAddressesAll = m_networkOperations->GetLocalIpAddresses();
        for (auto ipAddressResolved : ipAddressesAll) {
            AddIpAddress(std::move(ipAddressResolved));
        }

        return *this;
    }

    // Standard, fixed IP address.
    auto ipAddressInfo = m_networkOperations->GetLocalIpInformation(ipAddress);
    m_ipAddresses[ipAddress] = std::move(ipAddressInfo);

    return *this;
}

std::shared_ptr<NetRemoteDiscoveryService>
NetRemoteDiscoveryServiceBuilder::Build()
{
    return m_discoveryServiceFactory->Create(m_hostname, m_port, m_ipAddresses);
}
