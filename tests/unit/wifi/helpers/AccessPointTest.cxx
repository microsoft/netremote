
#include <microsoft/net/wifi/test/AccessPointTest.hxx>
#include <microsoft/net/wifi/test/AccessPointControllerTest.hxx>

using namespace Microsoft::Net::Wifi;
using namespace Microsoft::Net::Wifi::Test;

AccessPointTest::AccessPointTest(std::string_view interfaceName) :
    InterfaceName(interfaceName)
{}

std::string_view
AccessPointTest::GetInterface() const noexcept
{
    return InterfaceName;
}

std::unique_ptr<IAccessPointController>
AccessPointTest::CreateController()
{
    return std::make_unique<AccessPointControllerTest>(InterfaceName);
}

std::shared_ptr<IAccessPoint>
AccessPointFactoryTest::Create(std::string_view interface)
{
    return std::make_shared<AccessPointTest>(interface);
}

