
#include <microsoft/net/wifi/AccessPointDiscoveryAgent.hxx>
#include <microsoft/net/wifi/IAccessPoint.hxx>

using namespace Microsoft::Net::Wifi;

AccessPointDiscoveryAgent::AccessPointDiscoveryAgent(std::unique_ptr<IAccessPointDiscoveryAgentOperations> operations) noexcept :
    m_operations{ std::move(operations) }
{
}

AccessPointDiscoveryAgent::~AccessPointDiscoveryAgent()
{
    // If the agent was started, the operations object has a lambda that
    // captured the 'this' pointer. Once the agent is destroyed, the 'this'
    // pointer will become invalid. Consequently, call Stop() here to ensure the
    // operations object releases the lambda and no longer references the 'this'
    // pointer. Technically, the lambda could be retained by the operations
    // instance, however, we assume a well-behaved implementation in this
    // regard.
    Stop();
}

void
AccessPointDiscoveryAgent::RegisterDiscoveryEventCallback(std::function<void(AccessPointPresenceEvent presence, const std::shared_ptr<IAccessPoint> accessPointChanged)> onDevicePresenceChanged)
{
    std::unique_lock<std::shared_mutex> onDevicePresenceChangedLock{ m_onDevicePresenceChangedGate };
    m_onDevicePresenceChanged = std::move(onDevicePresenceChanged);
}

void
AccessPointDiscoveryAgent::DevicePresenceChanged(AccessPointPresenceEvent presence, std::shared_ptr<IAccessPoint> accessPointChanged) const noexcept
{
    std::shared_lock<std::shared_mutex> onDevicePresenceChangedLock{ m_onDevicePresenceChangedGate };
    if (m_onDevicePresenceChanged) {
        m_onDevicePresenceChanged(presence, std::move(accessPointChanged));
    }
}

bool
AccessPointDiscoveryAgent::IsStarted() const noexcept
{
    return m_started;
}

void
AccessPointDiscoveryAgent::Start()
{
    bool expected = false;
    if (m_started.compare_exchange_weak(expected, true)) {
        m_operations->Start([this](auto&& presence, auto&& accessPointChanged) {
            DevicePresenceChanged(presence, std::move(accessPointChanged));
        });
    }
}

void
AccessPointDiscoveryAgent::Stop()
{
    bool expected = true;
    if (m_started.compare_exchange_weak(expected, false)) {
        m_operations->Stop();
    }
}

std::future<std::vector<std::shared_ptr<IAccessPoint>>>
AccessPointDiscoveryAgent::ProbeAsync()
{
    return m_operations->ProbeAsync();
}
