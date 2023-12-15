
#include <microsoft/net/wifi/AccessPointDiscoveryAgent.hxx>
#include <microsoft/net/wifi/IAccessPoint.hxx>

using namespace Microsoft::Net::Wifi;

AccessPointDiscoveryAgent::AccessPointDiscoveryAgent(std::function<void(AccessPointPresenceEvent presence, const std::shared_ptr<IAccessPoint> deviceChanged)> onDevicePresenceChanged) :
    m_onDevicePresenceChanged(std::move(onDevicePresenceChanged))
{}

void AccessPointDiscoveryAgent::RegisterDiscoveryEventCallback(std::function<void(AccessPointPresenceEvent presence, const std::shared_ptr<IAccessPoint> deviceChanged)> onDevicePresenceChanged)
{
    std::unique_lock<std::shared_mutex> onDevicePresenceChangedLock{ m_onDevicePresenceChangedGate };
    m_onDevicePresenceChanged = std::move(onDevicePresenceChanged);
}

void AccessPointDiscoveryAgent::DevicePresenceChanged(AccessPointPresenceEvent presence, std::shared_ptr<IAccessPoint> deviceChanged) const noexcept
{
    std::shared_lock<std::shared_mutex> onDevicePresenceChangedLock{ m_onDevicePresenceChangedGate };
    if (m_onDevicePresenceChanged) 
    {
        m_onDevicePresenceChanged(presence, std::move(deviceChanged));
    }
}

bool AccessPointDiscoveryAgent::IsStarted() const noexcept
{
    return m_started;
}

void AccessPointDiscoveryAgent::Start()
{
    bool expected = false;
    if (m_started.compare_exchange_weak(expected, true)) 
    {
        StartImpl();
    }
}

void AccessPointDiscoveryAgent::Stop()
{
    bool expected = true;
    if (m_started.compare_exchange_weak(expected, false))
    {
        StopImpl();
    }
}

std::future<std::vector<std::shared_ptr<IAccessPoint>>> AccessPointDiscoveryAgent::ProbeAsync()
{
    return ProbeAsyncImpl();
}

void AccessPointDiscoveryAgent::StartImpl()
{}

void AccessPointDiscoveryAgent::StopImpl()
{}

std::future<std::vector<std::shared_ptr<IAccessPoint>>> AccessPointDiscoveryAgent::ProbeAsyncImpl()
{
    std::promise<std::vector<std::shared_ptr<IAccessPoint>>> probePromise{};
    probePromise.set_value({});
    return probePromise.get_future();
}
