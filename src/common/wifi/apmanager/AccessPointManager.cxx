
#include <algorithm>
#include <chrono>
#include <iterator>

#include <microsoft/net/wifi/AccessPoint.hxx>
#include <microsoft/net/wifi/AccessPointDiscoveryAgent.hxx>
#include <microsoft/net/wifi/AccessPointManager.hxx>
#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <notstd/Memory.hxx>

using namespace Microsoft::Net::Wifi;

AccessPointManager::AccessPointManager(std::unique_ptr<IAccessPointFactory> accessPointFactory) :
    m_accessPointFactory(std::move(accessPointFactory))
{}

/* static */
std::shared_ptr<AccessPointManager>
AccessPointManager::Create(std::unique_ptr<IAccessPointFactory> accessPointFactory)
{
    return std::make_shared<notstd::enable_make_protected<AccessPointManager>>(std::move(accessPointFactory));
}

std::shared_ptr<AccessPointManager>
AccessPointManager::GetInstance() noexcept
{
    return shared_from_this();
}

void
AccessPointManager::AddAccessPoint(std::shared_ptr<IAccessPoint> accessPoint)
{
    const auto accessPointsLock = std::scoped_lock{ m_accessPointGate };
    const auto accessPointExists = std::ranges::any_of(m_accessPoints, [&](const auto& accessPointExisting) {
        return (accessPointExisting->GetInterfaceName() == accessPoint->GetInterfaceName());
    });

    if (accessPointExists) {
        return;
    }

    m_accessPoints.push_back(std::move(accessPoint));
}

void
AccessPointManager::RemoveAccessPoint(std::shared_ptr<IAccessPoint> accessPoint)
{
    const auto accessPointsLock = std::scoped_lock{ m_accessPointGate };
    const auto accessPointToRemove = std::ranges::find_if(m_accessPoints, [&](const auto& accessPointExisting) {
        return (accessPointExisting->GetInterfaceName() == accessPoint->GetInterfaceName());
    });

    if (accessPointToRemove == std::cend(m_accessPoints)) {
        return;
    }

    m_accessPoints.erase(accessPointToRemove);
}

std::weak_ptr<IAccessPoint>
AccessPointManager::GetAccessPoint(std::string_view interfaceName) const
{
    const auto accessPointsLock = std::scoped_lock{ m_accessPointGate };
    const auto accessPoint = std::ranges::find_if(m_accessPoints, [&](const auto& accessPointExisting) {
        return (accessPointExisting->GetInterfaceName() == interfaceName);
    });

    if (accessPoint == std::cend(m_accessPoints)) {
        return {};
    }

    return *accessPoint;
}

std::vector<std::weak_ptr<IAccessPoint>>
AccessPointManager::GetAllAccessPoints() const
{
    const auto accessPointsLock = std::scoped_lock{ m_accessPointGate };

    std::vector<std::weak_ptr<IAccessPoint>> accessPoints(std::size(m_accessPoints));
    std::ranges::transform(m_accessPoints, std::begin(accessPoints), [](const auto& accessPoint) {
        // Implicit conversion from std::shared_ptr to std::weak_ptr.
        return accessPoint;
    });

    return accessPoints;
}

void
AccessPointManager::AddDiscoveryAgent(std::shared_ptr<AccessPointDiscoveryAgent> discoveryAgent)
{
    using namespace std::chrono_literals;

    // Register a discovery event callback.
    //
    // Use a weak_ptr below to ensure that the access point manager can
    // be safely destroyed prior to the discovery agent. This allows the
    // callback to be registered indefinitely, safely checking whether this
    // instance is still valid upon each callback invocation.
    discoveryAgent->RegisterDiscoveryEventCallback([discoveryAgentPtr = discoveryAgent.get(), weakThis = std::weak_ptr<AccessPointManager>(GetInstance())](auto&& presence, auto&& interfaceName) {
        if (auto strongThis = weakThis.lock()) {
            auto accessPointChanged = strongThis->m_accessPointFactory->Create(interfaceName);
            strongThis->OnAccessPointPresenceChanged(discoveryAgentPtr, presence, std::move(accessPointChanged));
        }
    });

    // Start the agent if not done already.
    if (!discoveryAgent->IsStarted()) {
        discoveryAgent->Start();
    }

    // Kick off a probe to ensure any access points already present will be added to this manager.
    auto existingAccessPointInterfaceNamesProbe = discoveryAgent->ProbeAsync();

    // Add the agent.
    {
        std::unique_lock<std::shared_mutex> discoveryAgentLock{ m_discoveryAgentsGate };
        m_discoveryAgents.push_back(std::move(discoveryAgent));
    }

    if (existingAccessPointInterfaceNamesProbe.valid()) {
        static constexpr auto ProbeTimeout = 3s;

        // Wait for the operation to complete.
        const auto waitResult = existingAccessPointInterfaceNamesProbe.wait_for(ProbeTimeout);

        // If the operation completed, get the results and add those access points.
        if (waitResult == std::future_status::ready) {
            auto existingAccessPointInterfaceNames = existingAccessPointInterfaceNamesProbe.get();
            for (const auto& existingAccessPointInterfaceName : existingAccessPointInterfaceNames) {
                auto existingAccessPoint = m_accessPointFactory->Create(existingAccessPointInterfaceName);
                AddAccessPoint(std::move(existingAccessPoint));
            }
        } else {
            // TODO: log error
        }
    }
}

void
AccessPointManager::OnAccessPointPresenceChanged(AccessPointDiscoveryAgent* /* discoveryAgent */, AccessPointPresenceEvent presence, std::shared_ptr<IAccessPoint> accessPointChanged)
{
    switch (presence) {
    case AccessPointPresenceEvent::Arrived:
        AddAccessPoint(std::move(accessPointChanged));
        break;
    case AccessPointPresenceEvent::Departed:
        RemoveAccessPoint(accessPointChanged);
        break;
    default:
        break;
    }
}
