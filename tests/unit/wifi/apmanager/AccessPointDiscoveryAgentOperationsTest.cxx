
#include <algorithm>

#include "AccessPointDiscoveryAgentOperationsTest.hxx"

using namespace Microsoft::Net::Wifi;
using namespace Microsoft::Net::Wifi::Test;
using Microsoft::Net::Wifi::AccessPointPresenceEvent;

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
        return m_accessPointInterfaceNames;
    });
}

void
AccessPointDiscoveryAgentOperationsTest::AddAccessPoint(std::string_view interfaceNameToAdd)
{
    m_accessPointInterfaceNames.push_back(std::string(interfaceNameToAdd));

    if (m_callback != nullptr) {
        m_callback(AccessPointPresenceEvent::Arrived, m_accessPointInterfaceNames.back());
    }
}

void
AccessPointDiscoveryAgentOperationsTest::RemoveAccessPoint(std::string_view interfaceNameToRemove)
{
    auto interfaceNameToRemoveIterator = std::ranges::find_if(m_accessPointInterfaceNames, [&](const auto& interfaceName) {
        return (interfaceNameToRemove == interfaceName);
    });

    if (interfaceNameToRemoveIterator != std::end(m_accessPointInterfaceNames)) {
        if (m_callback != nullptr) {
            m_callback(Microsoft::Net::Wifi::AccessPointPresenceEvent::Departed, *interfaceNameToRemoveIterator);
        }

        m_accessPointInterfaceNames.erase(interfaceNameToRemoveIterator);
    }
}
