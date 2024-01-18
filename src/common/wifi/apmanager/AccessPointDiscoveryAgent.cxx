
#include <microsoft/net/wifi/AccessPointDiscoveryAgent.hxx>
#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <notstd/Memory.hxx>
#include <plog/Log.h>

using namespace Microsoft::Net::Wifi;

AccessPointDiscoveryAgent::AccessPointDiscoveryAgent(std::unique_ptr<IAccessPointDiscoveryAgentOperations> operations) noexcept :
    m_operations{ std::move(operations) }
{
}

AccessPointDiscoveryAgent::~AccessPointDiscoveryAgent()
{
    Stop();
}

std::shared_ptr<AccessPointDiscoveryAgent>
AccessPointDiscoveryAgent::Create(std::unique_ptr<IAccessPointDiscoveryAgentOperations> operations)
{
    return std::make_shared<notstd::enable_make_protected<AccessPointDiscoveryAgent>>(std::move(operations));
}

std::shared_ptr<AccessPointDiscoveryAgent>
AccessPointDiscoveryAgent::GetInstance() noexcept
{
    return shared_from_this();
}

void
AccessPointDiscoveryAgent::RegisterDiscoveryEventCallback(AccessPointPresenceEventCallback onDevicePresenceChanged)
{
    std::unique_lock<std::shared_mutex> onDevicePresenceChangedLock{ m_onDevicePresenceChangedGate };
    m_onDevicePresenceChanged = std::move(onDevicePresenceChanged);
}

void
AccessPointDiscoveryAgent::DevicePresenceChanged(AccessPointPresenceEvent presence, std::string interfaceName) const noexcept
{
    std::shared_lock<std::shared_mutex> onDevicePresenceChangedLock{ m_onDevicePresenceChangedGate };
    if (m_onDevicePresenceChanged) {
        LOGD << "Access point discovery agent detected a device presence change";
        m_onDevicePresenceChanged(presence, std::move(interfaceName));
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
        LOGD << "Access point discovery agent starting";
        m_operations->Start([weakThis = std::weak_ptr<AccessPointDiscoveryAgent>(GetInstance())](auto&& presence, auto&& interfaceName) {
            // Attempt to promote the weak pointer to a shared pointer to ensure this instance is still valid.
            if (auto strongThis = weakThis.lock(); strongThis) {
                strongThis->DevicePresenceChanged(presence, std::move(interfaceName));
            }
        });
    }
}

void
AccessPointDiscoveryAgent::Stop()
{
    bool expected = true;
    if (m_started.compare_exchange_weak(expected, false)) {
        LOGD << "Access point discovery agent stopping";
        m_operations->Stop();
    }
}

std::future<std::vector<std::string>>
AccessPointDiscoveryAgent::ProbeAsync()
{
    LOGD << "Access point discovery agent probing for devices";
    return m_operations->ProbeAsync();
}
