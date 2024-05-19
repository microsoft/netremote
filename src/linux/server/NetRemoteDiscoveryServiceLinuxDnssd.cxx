
#include <format>

#include <plog/Log.h>

#include "NetRemoteDiscoveryServiceLinuxDnssd.hxx"
#include "SystemdResolvedDnssd.hxx"

using namespace Microsoft::Net::Remote;
using namespace Microsoft::Net::Systemd;

namespace Systemd
{
using Microsoft::Net::Systemd::ResolvedDnssd;
} // namespace Systemd

NetRemoteDiscoveryServiceLinuxDnssd::NetRemoteDiscoveryServiceLinuxDnssd(NetRemoteDiscoveryServiceConfiguration discoveryServiceConfiguration) :
    NetRemoteDiscoveryService(std::move(discoveryServiceConfiguration)),
    m_txtDataRecord(Systemd::ResolvedDnssd::BuildTxtDataRecord(GetIpAddresses()))
{}

void
NetRemoteDiscoveryServiceLinuxDnssd::Start()
{
    LOGI << "Starting systemd-resolved DNS-SD service registration";

    if (!std::empty(m_dbusServiceObjectPath)) {
        LOGW << std::format("Unregistering pre-existing DNS-SD dbus service with path {}", m_dbusServiceObjectPath);
        Stop();
    }

    auto dbusServiceObjectPath = Systemd::ResolvedDnssd::RegisterService(GetServiceName(), GetProtocol(), GetPort(), m_txtDataRecord);
    if (std::empty(dbusServiceObjectPath)) {
        LOGE << "Failed to register new DNS-SD service on dbus";
        return;
    }

    LOGI << std::format("Successfully registered DNS-SD dbus service object with path {}", dbusServiceObjectPath);
    m_dbusServiceObjectPath = std::move(dbusServiceObjectPath);
}

void
NetRemoteDiscoveryServiceLinuxDnssd::Stop()
{
    LOGI << "Stopping systemd-resolved DNS-SD service registration";

    if (std::empty(m_dbusServiceObjectPath)) {
        LOGE << "No DNS-SD dbus service found to unregister";
        return;
    }

    const auto unregisterSucceeded = Systemd::ResolvedDnssd::UnregisterService(m_dbusServiceObjectPath);
    if (!unregisterSucceeded) {
        LOGE << std::format("Failed to unregister DNS-SD dbus service with path {}", m_dbusServiceObjectPath);
        return;
    }

    LOGI << std::format("Successfully unregistered DNS-SD dbus service with path {}", m_dbusServiceObjectPath);
    m_dbusServiceObjectPath.clear();
}

std::unique_ptr<NetRemoteDiscoveryService>
NetRemoteDiscoveryServiceLinuxDnssdFactory::Create(NetRemoteDiscoveryServiceConfiguration discoveryServiceConfiguration)
{
    return std::make_unique<NetRemoteDiscoveryServiceLinuxDnssd>(std::move(discoveryServiceConfiguration));
}
