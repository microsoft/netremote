
#include <microsoft/net/wifi/test/AccessPointControllerTest.hxx>
#include <microsoft/net/wifi/test/AccessPointTest.hxx>

using namespace Microsoft::Net::Wifi;
using namespace Microsoft::Net::Wifi::Test;

AccessPointTest::AccessPointTest(std::string_view interfaceName) :
    AccessPointTest(interfaceName, Ieee80211AccessPointCapabilities{})
{}

AccessPointTest::AccessPointTest(std::string_view interfaceName, Microsoft::Net::Wifi::Ieee80211AccessPointCapabilities capabilities) :
    InterfaceName(interfaceName),
    Capabilities(capabilities)
{}

std::string_view
AccessPointTest::GetInterfaceName() const
{
    return InterfaceName;
}

std::unique_ptr<IAccessPointController>
AccessPointTest::CreateController()
{
    return std::make_unique<AccessPointControllerTest>(this);
}

std::shared_ptr<IAccessPoint>
AccessPointFactoryTest::Create(std::string_view interfaceName)
{
    return Create(interfaceName, nullptr);
}

std::shared_ptr<IAccessPoint>
AccessPointFactoryTest::Create(std::string_view interfaceName, [[maybe_unused]] std::unique_ptr<IAccessPointCreateArgs> createArgs)
{
    return std::make_shared<AccessPointTest>(interfaceName);
}
