
#ifndef ACCESS_POINT_DISCOVERY_AGENT_TEST_HXX
#define ACCESS_POINT_DISCOVERY_AGENT_TEST_HXX

#include <future>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPointDiscoveryAgentOperations.hxx>

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

    std::future<std::vector<std::string>>
    ProbeAsync() override;

    void
    AddAccessPoint(std::string_view accessPointInterfaceNameToAdd);

    void
    RemoveAccessPoint(std::string_view accessPointInterfaceNameToRemove);

private:
    AccessPointPresenceEventCallback m_callback;
    std::vector<std::string> m_accessPointInterfaceNames;
};

} // namespace Microsoft::Net::Wifi::Test

#endif // ACCESS_POINT_DISCOVERY_AGENT_TEST_HXX
