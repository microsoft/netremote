
#include <algorithm>
#include <chrono>
#include <iterator>

#include <microsoft/net/wifi/AccessPointDiscoveryAgent.hxx>
#include <microsoft/net/wifi/AccessPointManager.hxx>
#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <notstd/Memory.hxx>

using namespace Microsoft::Net::Wifi;

AccessPointManager::AccessPointManager() = default;

std::shared_ptr<AccessPointManager>
AccessPointManager::Create()
{
    return std::make_shared<notstd::enable_make_protected<AccessPointManager>>();
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
    const auto accessPointExists = std::any_of(std::cbegin(m_accessPoints), std::cend(m_accessPoints), [&](const auto& accessPointExisting) {
        return (accessPointExisting->GetInterface() == accessPoint->GetInterface());
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
    const auto accessPointToRemove = std::find_if(std::cbegin(m_accessPoints), std::cend(m_accessPoints), [&](const auto& accessPointExisting) {
        return (accessPointExisting->GetInterface() == accessPoint->GetInterface());
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
    const auto accessPoint = std::find_if(std::cbegin(m_accessPoints), std::cend(m_accessPoints), [&](const auto& accessPointExisting) {
        return (accessPointExisting->GetInterface() == interfaceName);
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
AccessPointManager::AddDiscoveryAgent(std::unique_ptr<AccessPointDiscoveryAgent> discoveryAgent)
{
    using namespace std::chrono_literals;

    // Use a weak_ptr below to ensure that the access point manager can
    // be safely destroyed prior to the discovery agent. This allows the
    // callback to be registered indefinitely, safely checking whether this
    // instance is still valid upon each callback invocation.
    discoveryAgent->RegisterDiscoveryEventCallback([discoveryAgentPtr = discoveryAgent.get(), weakThis = std::weak_ptr<AccessPointManager>(GetInstance())](auto&& presence, auto&& accessPointChanged) {
        if (auto strongThis = weakThis.lock()) {
            strongThis->OnAccessPointPresenceChanged(discoveryAgentPtr, presence, accessPointChanged);
        }
    });

    // If this agent has already started, kick off a probe to ensure any access
    // points already found will be added to this manager.
    std::future<std::vector<std::shared_ptr<IAccessPoint>>> existingAccessPointsProbe;
    const bool isStarted = discoveryAgent->IsStarted();
    if (isStarted) {
        existingAccessPointsProbe = discoveryAgent->ProbeAsync();
    } else {
        discoveryAgent->Start();
    }

    // If the agent hasn't yet been started, start it now, then probe for
    // existing access points in case they've already been discovered.
    {
        std::unique_lock<std::shared_mutex> discoveryAgentLock{ m_discoveryAgentsGate };
        m_discoveryAgents.push_back(std::move(discoveryAgent));
    }

    if (existingAccessPointsProbe.valid()) {
        static constexpr auto probeTimeout = 3s;

        // Wait for the operation to complete.
        const auto waitResult = existingAccessPointsProbe.wait_for(probeTimeout);

        // If the operation completed, get the results and add those access points.
        if (waitResult == std::future_status::ready) {
            auto existingAccessPoints = existingAccessPointsProbe.get();
            for (auto& existingAccessPoint : existingAccessPoints) {
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
