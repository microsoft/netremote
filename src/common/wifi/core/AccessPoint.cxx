
#include <stdexcept>

#include <microsoft/net/wifi/AccessPoint.hxx>

using namespace Microsoft::Net::Wifi;

AccessPoint::AccessPoint(std::string_view interfaceName, std::shared_ptr<IAccessPointControllerFactory> accessPointControllerFactory) :
    m_interfaceName(interfaceName),
    m_accessPointControllerFactory(std::move(accessPointControllerFactory))
{}

std::string_view
AccessPoint::GetInterfaceName() const
{
    return m_interfaceName;
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
