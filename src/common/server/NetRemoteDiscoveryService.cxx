
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
