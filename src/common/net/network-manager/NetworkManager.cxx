
#include <memory>
#include <string>
#include <unordered_map>

#include <microsoft/net/INetworkOperations.hxx>
#include <microsoft/net/NetworkManager.hxx>
#include <microsoft/net/wifi/AccessPointManager.hxx>

using namespace Microsoft::Net;
using namespace Microsoft::Net::Wifi;

NetworkManager::NetworkManager(std::unique_ptr<INetworkOperations> networkOperations, std::shared_ptr<AccessPointManager> accessPointManager) noexcept :
    m_networkOperations(std::move(networkOperations)),
    m_accessPointManager(std::move(accessPointManager))
{}

std::shared_ptr<AccessPointManager>
NetworkManager::GetAccessPointManager() const noexcept
{
    return m_accessPointManager;
}

std::unordered_map<Microsoft::Net::NetworkInterfaceId, std::unordered_set<Microsoft::Net::NetworkIpAddress>>
NetworkManager::GetNetworkInterfaceInformation() const noexcept
{
    std::unordered_map<NetworkInterfaceId, std::unordered_set<NetworkIpAddress>> networkInterfaceInfo{};

    auto networkInterfaces = m_networkOperations->EnumerateNetworkInterfaces();
    for (auto& networkInterfaceId : networkInterfaces) {
        auto networkIpAddresses = m_networkOperations->GetNetworkInterfaceAddresses(networkInterfaceId.Name);
        networkInterfaceInfo[std::move(networkInterfaceId)] = std::move(networkIpAddresses);
    }

    return networkInterfaceInfo;
}

std::unordered_map<std::string, IpAddressInformation>
NetworkManager::GetLocalIpAddressInformation(std::string_view ipAddress) const noexcept
{
    if (m_networkOperations == nullptr) {
        return {};
    }

    return m_networkOperations->GetLocalIpAddressInformation(ipAddress);
}
