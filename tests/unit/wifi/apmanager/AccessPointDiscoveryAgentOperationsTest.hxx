
#ifndef ACCESS_POINT_DISCOVERY_AGENT_TEST_HXX
#define ACCESS_POINT_DISCOVERY_AGENT_TEST_HXX

#include <future>
#include <memory>

#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPointDiscoveryAgentOperations.hxx>

#include "AccessPointTest.hxx"

namespace Microsoft::Net::Wifi::Test
{
/**
 * @brief Access point discovery operations for use in unit tests.
 */
struct AccessPointDiscoveryAgentOperationsTest :
    public Microsoft::Net::Wifi::IAccessPointDiscoveryAgentOperations
{
    void
    Start(AccessPointPresenceEventCallback callback) override;

    void
    Stop() override;

    std::future<std::vector<std::shared_ptr<Microsoft::Net::Wifi::IAccessPoint>>>
    ProbeAsync() override;

    void
    AddAccessPoint(std::shared_ptr<Microsoft::Net::Wifi::IAccessPoint> accessPointToAdd);

    void
    RemoveAccessPoint(std::shared_ptr<Microsoft::Net::Wifi::IAccessPoint> accessPointToRemove);

private:
    AccessPointPresenceEventCallback m_callback;
    std::vector<std::shared_ptr<Microsoft::Net::Wifi::IAccessPoint>> m_accessPoints;
};

} // namespace Microsoft::Net::Wifi::Test

#endif // ACCESS_POINT_DISCOVERY_AGENT_TEST_HXX
