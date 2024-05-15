
#include <format>
#include <future>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <utility>
#include <vector>

#include <magic_enum.hpp>
#include <microsoft/net/wifi/AccessPointDiscoveryAgent.hxx>
#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPointDiscoveryAgentOperations.hxx>
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
    const std::unique_lock<std::shared_mutex> onDevicePresenceChangedLock{ m_onDevicePresenceChangedGate };
    m_onDevicePresenceChanged = std::move(onDevicePresenceChanged);
}

void
AccessPointDiscoveryAgent::DevicePresenceChanged(AccessPointPresenceEvent presence, std::shared_ptr<IAccessPoint> accessPoint) const noexcept
{
    const std::shared_lock<std::shared_mutex> onDevicePresenceChangedLock{ m_onDevicePresenceChangedGate };
    if (m_onDevicePresenceChanged) {
        LOGI << std::format("Access Point Discovery Event: Interface {} {}", accessPoint->GetInterfaceName(), magic_enum::enum_name(presence));
        m_onDevicePresenceChanged(presence, std::move(accessPoint));
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
        LOGI << "Access point discovery agent starting";
        m_operations->Start([weakThis = std::weak_ptr<AccessPointDiscoveryAgent>(GetInstance())](auto&& presence, auto&& accessPoint) {
            // Attempt to promote the weak pointer to a shared pointer to ensure this instance is still valid.
            if (auto strongThis = weakThis.lock(); strongThis) {
                strongThis->DevicePresenceChanged(presence, std::move(accessPoint));
            } else {
                LOGW << "Access point discovery agent instance no longer valid; ignoring presence change event";
            }
        });
    }
}

void
AccessPointDiscoveryAgent::Stop()
{
    bool expected = true;
    if (m_started.compare_exchange_weak(expected, false)) {
        LOGI << "Access point discovery agent stopping";
        m_operations->Stop();
    }
}

std::future<std::vector<std::shared_ptr<IAccessPoint>>>
AccessPointDiscoveryAgent::ProbeAsync()
{
    LOGI << "Access point probe initiated";
    return m_operations->ProbeAsync();
}
