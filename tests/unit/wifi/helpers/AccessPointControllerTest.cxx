
#include <microsoft/net/wifi/test/AccessPointControllerTest.hxx>
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

AccessPointCapabilities2
AccessPointControllerTest::GetCapabilities()
{
    return {}; // TODO: return something
}

std::unique_ptr<IAccessPointController>
AccessPointControllerFactoryTest::Create(std::string_view interfaceName)
{
    return std::make_unique<AccessPointControllerTest>(interfaceName);
}
