
#include "AccessPointFactoryTest.hxx"

using namespace Microsoft::Net::Wifi;
using namespace Microsoft::Net::Wifi::Test;

std::shared_ptr<AccessPoint> AccessPointFactoryTest::Create(std::string_view interface)
{
    return std::make_shared<AccessPoint>(interface);
}
