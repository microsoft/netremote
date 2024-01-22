
#ifndef ACCESS_POINT_DISCOVERY_AGENT_TEST_HXX
#define ACCESS_POINT_DISCOVERY_AGENT_TEST_HXX

#include <future>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPointDiscoveryAgentOperations.hxx>
#include <microsoft/net/wifi/test/AccessPointTest.hxx>

namespace Microsoft::Net::Wifi::Test
{
/**
 * @brief Access point discovery operations for use in unit tests.
 */
struct AccessPointDiscoveryAgentOperationsTest :
    public Microsoft::Net::Wifi::IAccessPointDiscoveryAgentOperations
{
    AccessPointDiscoveryAgentOperationsTest();

    void
    Start(AccessPointPresenceEventCallback callback) override;

    void
    Stop() override;

    std::future<std::vector<std::string>>
    ProbeAsync() override;

    std::future<std::vector<std::shared_ptr<IAccessPoint>>>
    ProbeAsync2() override;

    void
    AddAccessPoint(std::string_view accessPointInterfaceNameToAdd);

    void
    RemoveAccessPoint(std::string_view accessPointInterfaceNameToRemove);

private:
    AccessPointPresenceEventCallback m_callback;
    std::vector<std::shared_ptr<IAccessPoint>> m_accessPoints;
    std::unique_ptr<IAccessPointFactory> m_accessPointFactory;
};

} // namespace Microsoft::Net::Wifi::Test

#endif // ACCESS_POINT_DISCOVERY_AGENT_TEST_HXX
