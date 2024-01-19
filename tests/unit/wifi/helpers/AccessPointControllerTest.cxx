
#include <microsoft/net/wifi/test/AccessPointControllerTest.hxx>

using namespace Microsoft::Net::Wifi;
using namespace Microsoft::Net::Wifi::Test;

AccessPointControllerTest::AccessPointControllerTest(std::string_view interfaceName) :
    InterfaceName(interfaceName)
{}

std::string_view
AccessPointControllerTest::GetInterfaceName() const noexcept
{
    return InterfaceName;
}

Ieee80211AccessPointCapabilities
AccessPointControllerTest::GetCapabilities()
{
    return {}; // TODO: return something
}

std::unique_ptr<IAccessPointController>
AccessPointControllerFactoryTest::Create(std::string_view interfaceName)
{
    return std::make_unique<AccessPointControllerTest>(interfaceName);
}
