
#include <microsoft/net/wifi/AccessPointManager.hxx>

using namespace Microsoft::Net::Wifi;

AccessPointManager::AccessPointManager(std::unique_ptr<IAccessPointFactory> accessPointFactory) :
    m_accessPointFactory{ std::move(accessPointFactory) }
{
}

std::unordered_map<std::string, std::weak_ptr<AccessPoint>> AccessPointManager::EnumerateAccessPoints() const noexcept
{
    std::shared_lock sharedAccessPointsLock{ m_accessPointsGate };
    return { std::cbegin(m_accessPoints), std::cend(m_accessPoints) };
}
