
#include <algorithm>

#include "AccessPointDiscoveryAgentOperationsTest.hxx"

using namespace Microsoft::Net::Wifi;
using namespace Microsoft::Net::Wifi::Test;
using Microsoft::Net::Wifi::AccessPointPresenceEvent;

AccessPointDiscoveryAgentOperationsTest::AccessPointDiscoveryAgentOperationsTest() :
    m_accessPointFactory(std::make_unique<AccessPointFactoryTest>())
{}

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

std::future<std::vector<std::string>>
AccessPointDiscoveryAgentOperationsTest::ProbeAsync()
{
    return std::async(std::launch::async, [&]() {
        // return m_accessPointInterfaceNames;
        return std::vector<std::string>();
    });
}

std::future<std::vector<std::shared_ptr<IAccessPoint>>>
AccessPointDiscoveryAgentOperationsTest::ProbeAsync2()
{
    return std::async(std::launch::async, [&]() {
        return m_accessPoints;
    });
}

void
AccessPointDiscoveryAgentOperationsTest::AddAccessPoint(std::string_view interfaceNameToAdd)
{
    auto accessPointToAdd = m_accessPointFactory->Create(interfaceNameToAdd);
    m_accessPoints.push_back(accessPointToAdd);

    if (m_callback != nullptr) {
        m_callback(AccessPointPresenceEvent::Arrived, m_accessPoints.back());
    }
}

void
AccessPointDiscoveryAgentOperationsTest::RemoveAccessPoint(std::string_view interfaceNameToRemove)
{
    auto accessPointToRemoveIterator = std::ranges::find_if(m_accessPoints, [&](const auto& accessPoint) {
        return (interfaceNameToRemove == accessPoint->GetInterfaceName());
    });

    if (accessPointToRemoveIterator != std::end(m_accessPoints)) {
        if (m_callback != nullptr) {
            m_callback(Microsoft::Net::Wifi::AccessPointPresenceEvent::Departed, *accessPointToRemoveIterator);
        }

        m_accessPoints.erase(accessPointToRemoveIterator);
    }
}
