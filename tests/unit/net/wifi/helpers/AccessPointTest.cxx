#include <memory>
#include <string_view>
#include <utility>

#include <microsoft/net/wifi/IAccessPoint.hxx>
#include <microsoft/net/wifi/IAccessPointController.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <microsoft/net/wifi/Ieee80211AccessPointCapabilities.hxx>
#include <microsoft/net/wifi/test/AccessPointControllerTest.hxx>
#include <microsoft/net/wifi/test/AccessPointTest.hxx>

using namespace Microsoft::Net::Wifi;
using namespace Microsoft::Net::Wifi::Test;

AccessPointTest::AccessPointTest(std::string_view interfaceName, Ieee80211AccessPointCapabilities capabilities, AccessPointAttributes attributes) :
    InterfaceName(interfaceName),
    Capabilities(std::move(capabilities)),
    Attributes(std::move(attributes))
{}

std::string_view
AccessPointTest::GetInterfaceName() const noexcept
{
    return InterfaceName;
}

Ieee80211MacAddress
AccessPointTest::GetMacAddress() const noexcept
{
    return MacAddress;
}

const AccessPointAttributes&
AccessPointTest::GetAttributes() const noexcept
{
    return Attributes;
}

std::unique_ptr<IAccessPointController>
AccessPointTest::CreateController()
{
    return std::make_unique<AccessPointControllerTest>(this);
}

std::shared_ptr<IAccessPoint>
AccessPointFactoryTest::Create(std::string_view interfaceName, [[maybe_unused]] std::unique_ptr<IAccessPointCreateArgs> createArgs)
{
    return std::make_shared<AccessPointTest>(interfaceName);
}
