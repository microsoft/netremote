
#include <microsoft/net/wifi/AccessPointManager.hxx>

using namespace Microsoft::Net::Wifi;

std::unordered_map<std::string, AccessPoint> AccessPointManager::EnumerateAccessPoints() const noexcept
{
    std::shared_lock sharedAccessPointsLock{ m_accessPointsGate };
    return m_accessPoints;
}
