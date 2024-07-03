
#include <memory>
#include <utility>

#include <microsoft/net/wifi/AccessPointManager.hxx>
#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <microsoft/net/wifi/test/AccessPointManagerTest.hxx>

using namespace Microsoft::Net::Wifi::Test;

void
AccessPointManagerTest::AddAccessPoint(std::shared_ptr<IAccessPoint> accessPoint)
{
    return AccessPointManager::AddAccessPoint(std::move(accessPoint));
}

void
AccessPointManagerTest::RemoveAccessPoint(std::shared_ptr<IAccessPoint> accessPoint)
{
    return AccessPointManager::RemoveAccessPoint(std::move(accessPoint));
}
