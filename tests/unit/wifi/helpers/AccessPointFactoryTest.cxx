
#include <microsoft/net/wifi/AccessPoint.hxx>
#include <microsoft/net/wifi/test/AccessPointFactoryTest.hxx>

using namespace Microsoft::Net::Wifi;
using namespace Microsoft::Net::Wifi::Test;

std::shared_ptr<IAccessPoint>
AccessPointFactoryTest::Create(std::string_view interface)
{
    return std::make_shared<AccessPoint>(interface);
}
