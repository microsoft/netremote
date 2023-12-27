
#include <microsoft/net/wifi/AccessPointDiscoveryAgentOperationsNetlink.hxx>

using namespace Microsoft::Net::Wifi;

void
AccessPointDiscoveryAgentOperationsNetlink::Start([[maybe_unused]] AccessPointPresenceEventCallback callback)
{
}

void
AccessPointDiscoveryAgentOperationsNetlink::Stop()
{
}

std::future<std::vector<std::shared_ptr<IAccessPoint>>>
AccessPointDiscoveryAgentOperationsNetlink::ProbeAsync()
{
    return {};
}
