
#include <memory>
#include <string_view>
#include <utility>

#include <microsoft/net/remote/INetRemoteCliHandlerOperations.hxx>
#include <microsoft/net/remote/NetRemoteCli.hxx>
#include <microsoft/net/remote/NetRemoteCliHandler.hxx>
#include <microsoft/net/remote/NetRemoteServerConnection.hxx>
#include <microsoft/net/wifi/Ieee80211AccessPointConfiguration.hxx>
#include <plog/Log.h>

using namespace Microsoft::Net;
using namespace Microsoft::Net::Remote;
using namespace Microsoft::Net::Wifi;

NetRemoteCliHandler::NetRemoteCliHandler(std::unique_ptr<INetRemoteCliHandlerOperationsFactory> operationsFactory) :
    m_operationsFactory(std::move(operationsFactory))
{
}

void
NetRemoteCliHandler::SetParent(std::weak_ptr<NetRemoteCli> parent)
{
    m_parent = std::move(parent);
}

void
NetRemoteCliHandler::SetConnection(std::shared_ptr<NetRemoteServerConnection> connection)
{
    m_connection = std::move(connection);
    m_operations = m_operationsFactory->Create(m_connection);
}

std::shared_ptr<NetRemoteCli>
NetRemoteCliHandler::GetParentStrongRef() const
{
    return m_parent.lock();
}

std::tuple<std::shared_ptr<NetRemoteCli>, std::shared_ptr<INetRemoteCliHandlerOperations>>
NetRemoteCliHandler::GetOperationsAndParentStrongRef() const
{
    auto parentStrong{ GetParentStrongRef() };
    if (!parentStrong) {
        LOGE << "Parent cli object is no longer valid, aborting command";
        return {};
    }

    auto operations{ m_operations };
    if (!operations) {
        LOGE << "No operations instance available to handle command";
        return {};
    }

    return { parentStrong, operations };
}

void
NetRemoteCliHandler::HandleCommandNetworkInterfacesEnumerate()
{
    auto [parentStrong, operations] = GetOperationsAndParentStrongRef();
    if (!parentStrong || !operations) {
        return;
    }

    LOGD << "Executing command NetworkInterfacesEnumerate";
    operations->NetworkInterfacesEnumerate();
}

void
NetRemoteCliHandler::HandleCommandWifiAccessPointsEnumerate(bool detailedOutput)
{
    auto [parentStrong, operations] = GetOperationsAndParentStrongRef();
    if (!parentStrong || !operations) {
        return;
    }

    LOGD << "Executing command WifiAccessPointsEnumerate";
    operations->WifiAccessPointsEnumerate(detailedOutput);
}

void
NetRemoteCliHandler::HandleCommandWifiAccessPointEnable(std::string_view accessPointId, const Ieee80211AccessPointConfiguration* ieee80211AccessPointConfiguration)
{
    auto [parentStrong, operations] = GetOperationsAndParentStrongRef();
    if (!parentStrong || !operations) {
        return;
    }

    LOGD << "Executing command WifiAccessPointEnable";
    operations->WifiAccessPointEnable(accessPointId, ieee80211AccessPointConfiguration);
}

void
NetRemoteCliHandler::HandleCommandWifiAccessPointDisable(std::string_view accessPointId)
{
    auto [parentStrong, operations] = GetOperationsAndParentStrongRef();
    if (!parentStrong || !operations) {
        return;
    }

    LOGD << "Executing command WifiAccessPointDisable";
    operations->WifiAccessPointDisable(accessPointId);
}

void
NetRemoteCliHandler::HandleCommandWifiAccessPointSetSsid(std::string_view accessPointId, std::string_view ssid)
{
    auto [parentStrong, operations] = GetOperationsAndParentStrongRef();
    if (!parentStrong || !operations) {
        return;
    }

    LOGD << "Executing command WifiAccessPointSetSsid";
    operations->WifiAccessPointSetSsid(accessPointId, ssid);
}

void
NetRemoteCliHandler::HandleCommandWifiAccessPointSet8021xRadius(std::string_view accessPointId, const Ieee8021xRadiusConfiguration* ieee8021xRadiusConfiguration)
{
    auto [parentStrong, operations] = GetOperationsAndParentStrongRef();
    if (!parentStrong || !operations) {
        return;
    }

    LOGD << "Executing command WifiAccessPointSet8021xRadius";
    operations->WifiAccessPointSet8021xRadius(accessPointId, ieee8021xRadiusConfiguration);
}
