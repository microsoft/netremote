
#include <memory>

#include <microsoft/net/wifi/AccessPointLinux.hxx>
#include <microsoft/net/wifi/test/AccessPointControllerTest.hxx>
#include <microsoft/net/wifi/test/AccessPointFactoryLinuxTest.hxx>

using namespace Microsoft::Net::Wifi::Test;

AccessPointFactoryLinuxTest::AccessPointFactoryLinuxTest() :
    AccessPointFactoryLinux{ std::make_shared<AccessPointControllerFactoryTest>() }
{
}
