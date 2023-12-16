
#include <algorithm>

#include "AccessPointDiscoveryAgentOperationsTest.hxx"

using namespace Microsoft::Net::Wifi::Test;

void
AccessPointDiscoveryAgentOperationsTest::Start(AccessPointPresenceEventCallback callback)
{
    m_callback = callback;
}

void
AccessPointDiscoveryAgentOperationsTest::Stop()
{
    m_callback = nullptr;
}

std::future<std::vector<std::shared_ptr<Microsoft::Net::Wifi::IAccessPoint>>>
AccessPointDiscoveryAgentOperationsTest::ProbeAsync()
{
    return std::async(std::launch::async, [&]() {
        return m_accessPoints;
    });
}

void
AccessPointDiscoveryAgentOperationsTest::AddAccessPoint(std::shared_ptr<Microsoft::Net::Wifi::IAccessPoint> accessPointToAdd)
{
    m_accessPoints.push_back(std::move(accessPointToAdd));

    if (m_callback != nullptr) {
        m_callback(Microsoft::Net::Wifi::AccessPointPresenceEvent::Arrived, m_accessPoints.back());
    }
}

void
AccessPointDiscoveryAgentOperationsTest::RemoveAccessPoint(std::shared_ptr<Microsoft::Net::Wifi::IAccessPoint> accessPointToRemove)
{
    auto accessPointToRemoveIterator = std::ranges::find_if(m_accessPoints, [&](const auto& accessPoint) {
        return accessPointToRemove->GetInterface() == accessPoint->GetInterface();
    });

    if (accessPointToRemoveIterator != std::end(m_accessPoints)) {
        if (m_callback != nullptr) {
            m_callback(Microsoft::Net::Wifi::AccessPointPresenceEvent::Departed, *accessPointToRemoveIterator);
        }

        m_accessPoints.erase(accessPointToRemoveIterator);
    }
}
