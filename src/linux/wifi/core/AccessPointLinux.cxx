
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

AccessPointLinux::AccessPointLinux(std::string_view interfaceName, std::shared_ptr<IAccessPointControllerFactory> accessPointControllerFactory, Microsoft::Net::Netlink::Nl80211::Nl80211Interface nl80211Interface) :
    AccessPoint(interfaceName, std::move(accessPointControllerFactory)),
    m_nl80211Interface{ std::move(nl80211Interface) }
{
}

std::unique_ptr<IAccessPointController>
AccessPointLinux::CreateController()
{
    return std::make_unique<AccessPointControllerLinux>(GetInterfaceName());
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

    return std::make_shared<AccessPointLinux>(interfaceName, GetControllerFactory(), std::move(createArgsLinux->Interface));
}

AccessPointCreateArgsLinux::AccessPointCreateArgsLinux(Nl80211Interface nl80211Interface) :
    Interface{ std::move(nl80211Interface) }
{
}
