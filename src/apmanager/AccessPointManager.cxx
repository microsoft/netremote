
#include <microsoft/net/wifi/AccessPointManager.hxx>

using namespace Microsoft::Net::Wifi;

std::unordered_map<std::string, AccessPoint> AccessPointManager::EnumerateAccessPoints() const noexcept
{
    return m_accessPoints;
}
