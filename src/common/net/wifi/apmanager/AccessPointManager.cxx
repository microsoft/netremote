
#include <algorithm>
#include <chrono> // NOLINT(misc-include-cleaner)
#include <format>
#include <future>
#include <iterator>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string_view>
#include <utility>
#include <vector>

#include <logging/LogUtils.hxx>
#include <magic_enum.hpp>
#include <microsoft/net/wifi/AccessPointDiscoveryAgent.hxx>
#include <microsoft/net/wifi/AccessPointManager.hxx>
#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPointDiscoveryAgentOperations.hxx>
#include <notstd/Memory.hxx>
#include <plog/Log.h>

using namespace Microsoft::Net::Wifi;

AccessPointManager::AccessPointManager(std::unordered_map<std::string, AccessPointAttributes> accessPointAttributes) :
    m_accessPointAttributes{ std::move(accessPointAttributes) }
{}

/* static */
std::shared_ptr<AccessPointManager>
AccessPointManager::Create(std::unordered_map<std::string, AccessPointAttributes> accessPointAttributes)
{
    return std::make_shared<notstd::enable_make_protected<AccessPointManager>>(std::move(accessPointAttributes));
}

std::shared_ptr<AccessPointManager>
AccessPointManager::GetInstance() noexcept
{
    return shared_from_this();
}

void
AccessPointManager::AddAccessPoint(std::shared_ptr<IAccessPoint> accessPoint)
{
    const auto interfaceName{ accessPoint->GetInterfaceName() };
    LOGI << std::format("Attempting to add access point {} to manager", interfaceName);
    AUDITD << std::format("Attempting to add access point {} to manager", interfaceName);

    {
        auto accessPointController = accessPoint->CreateController();
        if (!accessPointController) {
            LOGW << std::format("Access point {} not added (not controllable)", interfaceName);
            return;
        }
    }

    const auto accessPointsLock = std::scoped_lock{ m_accessPointGate };
    const auto accessPointExists = std::ranges::any_of(m_accessPoints, [&](const auto& accessPointExisting) {
        return (accessPointExisting->GetInterfaceName() == interfaceName);
    });

    if (accessPointExists) {
        LOGW << std::format("Access point {} not added (already exists)", interfaceName);
        return;
    }

    LOGI << std::format("Adding access point {} to manager", interfaceName);
    AUDITI << std::format("Adding access point {} to manager", interfaceName);

    m_accessPoints.push_back(std::move(accessPoint));
}

void
AccessPointManager::RemoveAccessPoint(std::shared_ptr<IAccessPoint> accessPoint)
{
    const auto interfaceName{ accessPoint->GetInterfaceName() };
    LOGI << std::format("Attempting to remove access point {} from manager", interfaceName);
    AUDITD << std::format("Attempting to remove access point {} from manager", interfaceName);

    const auto accessPointsLock = std::scoped_lock{ m_accessPointGate };
    const auto accessPointToRemove = std::ranges::find_if(m_accessPoints, [&](const auto& accessPointExisting) {
        return (accessPointExisting->GetInterfaceName() == interfaceName);
    });

    if (accessPointToRemove == std::cend(m_accessPoints)) {
        LOGW << std::format("Access point {} not removed (not found in manager)", interfaceName);
        return;
    }

    LOGI << std::format("Removing access point {} from manager", interfaceName);
    AUDITI << std::format("Removing access point {} from manager", interfaceName);

    m_accessPoints.erase(accessPointToRemove);
}

std::optional<std::weak_ptr<IAccessPoint>>
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

const std::unordered_map<std::string, AccessPointAttributes>&
AccessPointManager::GetAllAccessPointAttributes() const
{
    return m_accessPointAttributes;
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
    discoveryAgent->RegisterDiscoveryEventCallback([discoveryAgentPtr = discoveryAgent.get(), weakThis = std::weak_ptr<AccessPointManager>(GetInstance())](auto&& presence, auto&& accessPointChanged) {
        if (auto strongThis = weakThis.lock()) {
            strongThis->OnAccessPointPresenceChanged(discoveryAgentPtr, presence, std::move(accessPointChanged));
        } else {
            LOGW << std::format("Access point manager no longer valid (expired); ignoring presence change event {} for {}", magic_enum::enum_name(presence), accessPointChanged->GetInterfaceName());
        }
    });

    // Start the agent if not done already.
    if (!discoveryAgent->IsStarted()) {
        discoveryAgent->Start();
    }

    // Kick off a probe to ensure any access points already present will be added to this manager.
    auto existingAccessPointsProbe = discoveryAgent->ProbeAsync();

    // Add the agent.
    {
        const std::unique_lock<std::shared_mutex> discoveryAgentLock{ m_discoveryAgentsGate };
        m_discoveryAgents.push_back(std::move(discoveryAgent));
    }

    if (existingAccessPointsProbe.valid()) {
        static constexpr auto ProbeTimeout = 3s;

        // Wait for the operation to complete.
        const auto waitResult = existingAccessPointsProbe.wait_for(ProbeTimeout);

        // If the operation completed, get the results and add those access points.
        if (waitResult == std::future_status::ready) {
            auto existingAccessPoints = existingAccessPointsProbe.get();
            for (auto& existingAccessPoint : existingAccessPoints) {
                AddAccessPoint(std::move(existingAccessPoint));
            }
        } else {
            LOGE << std::format("Access point discovery agent probe failed ({})", magic_enum::enum_name(waitResult));
        }
    }
}

void
AccessPointManager::OnAccessPointPresenceChanged(AccessPointDiscoveryAgent* /* discoveryAgent */, AccessPointPresenceEvent presence, std::shared_ptr<IAccessPoint> accessPointChanged)
{
    LOGI << std::format("Access point presence changed: {} ({})", accessPointChanged->GetInterfaceName(), magic_enum::enum_name(presence));

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
