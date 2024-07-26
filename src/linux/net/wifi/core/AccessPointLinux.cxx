
#include <memory>
#include <stdexcept>
#include <string_view>
#include <utility>

#include <microsoft/net/netlink/nl80211/Netlink80211Interface.hxx>
#include <microsoft/net/wifi/AccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPointController.hxx>

#include <microsoft/net/wifi/AccessPointControllerLinux.hxx>
#include <microsoft/net/wifi/AccessPointLinux.hxx>

using Microsoft::Net::Netlink::Nl80211::Nl80211Interface;

using namespace Microsoft::Net::Wifi;

AccessPointLinux::AccessPointLinux(std::string_view interfaceName, std::shared_ptr<IAccessPointControllerFactory> accessPointControllerFactory, Nl80211Interface nl80211Interface, AccessPointProperties properties) :
    AccessPoint(interfaceName, std::move(accessPointControllerFactory), std::move(properties)),
    m_nl80211Interface{ std::move(nl80211Interface) }
{
}

Ieee80211MacAddress
AccessPointLinux::GetMacAddress() const noexcept
{
    return m_nl80211Interface.MacAddress;
}

std::shared_ptr<IAccessPoint>
AccessPointFactoryLinux::Create(std::string_view interfaceName)
{
    return Create(interfaceName, nullptr);
}

std::shared_ptr<IAccessPoint>
AccessPointFactoryLinux::Create(std::string_view interfaceName, std::unique_ptr<IAccessPointCreateArgs> createArgs)
{
    auto *createArgsLinux = dynamic_cast<AccessPointCreateArgsLinux *>(createArgs.get());
    if (createArgsLinux == nullptr) {
        throw std::runtime_error("invalid arguments passed to AccessPointFactoryLinux::Create; this is a bug!");
    }

    return std::make_shared<AccessPointLinux>(interfaceName, GetControllerFactory(), std::move(createArgsLinux->Interface), std::move(createArgs->Properties));
}

AccessPointCreateArgsLinux::AccessPointCreateArgsLinux(Nl80211Interface nl80211Interface) :
    Interface{ std::move(nl80211Interface) }
{
}
