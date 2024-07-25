
#include <memory>
#include <optional>
#include <string_view>
#include <utility>

#include <microsoft/net/wifi/AccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPointController.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>

using namespace Microsoft::Net::Wifi;

AccessPoint::AccessPoint(std::string_view interfaceName, std::shared_ptr<IAccessPointControllerFactory> accessPointControllerFactory, std::optional<Ieee80211MacAddress> macAddress) :
    m_interfaceName(interfaceName),
    m_accessPointControllerFactory(std::move(accessPointControllerFactory)),
    m_macAddress(macAddress)
{}

std::string_view
AccessPoint::GetInterfaceName() const noexcept
{
    return m_interfaceName;
}

Ieee80211MacAddress
AccessPoint::GetMacAddress() const noexcept
{
    return m_macAddress.value_or(Ieee80211MacAddress{});
}

AccessPointProperties
AccessPoint::GetProperties() const noexcept
{
    return m_properties;
}

std::unique_ptr<IAccessPointController>
AccessPoint::CreateController()
{
    return (m_accessPointControllerFactory != nullptr)
        ? m_accessPointControllerFactory->Create(m_interfaceName)
        : nullptr;
}

AccessPointFactory::AccessPointFactory(std::shared_ptr<IAccessPointControllerFactory> accessPointControllerFactory) :
    m_accessPointControllerFactory(std::move(accessPointControllerFactory))
{}

std::shared_ptr<IAccessPointControllerFactory>
AccessPointFactory::GetControllerFactory() const noexcept
{
    return m_accessPointControllerFactory;
}

std::shared_ptr<IAccessPoint>
AccessPointFactory::Create(std::string_view interfaceName)
{
    return Create(interfaceName, nullptr);
}

std::shared_ptr<IAccessPoint>
AccessPointFactory::Create(std::string_view interfaceName, [[maybe_unused]] std::unique_ptr<IAccessPointCreateArgs> accessPointCreateArgs)
{
    return std::make_shared<AccessPoint>(interfaceName, GetControllerFactory());
}
