
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
    m_txtRecords({ Systemd::ResolvedDnssd::BuildTxtTextRecord(GetIpAddresses()) })
{}

void
NetRemoteDiscoveryServiceLinuxDnssd::Start()
{
    auto dbusServiceObjectPath = Systemd::ResolvedDnssd::RegisterService(GetServiceName(), GetProtocol(), GetPort(), m_txtRecords);
    m_dbusServiceObjectPath = std::move(dbusServiceObjectPath);
}

void
NetRemoteDiscoveryServiceLinuxDnssd::Stop()
{
    if (std::empty(m_dbusServiceObjectPath)) {
        LOGE << "No dbus service object path to unregister";
        return;
    }

    const auto unregisterSucceeded = Systemd::ResolvedDnssd::UnregisterService(m_dbusServiceObjectPath);
    if (!unregisterSucceeded) {
        LOGE << std::format("Failed to unregister dbus service object path {}", m_dbusServiceObjectPath);
        return;
    }

    LOGI << std::format("Successfully unregistered dbus service object path {}", m_dbusServiceObjectPath);
    m_dbusServiceObjectPath.clear();
}

std::unique_ptr<NetRemoteDiscoveryService>
NetRemoteDiscoveryServiceLinuxDnssdFactory::Create(NetRemoteDiscoveryServiceConfiguration discoveryServiceConfiguration)
{
    return std::make_unique<NetRemoteDiscoveryServiceLinuxDnssd>(std::move(discoveryServiceConfiguration));
}
