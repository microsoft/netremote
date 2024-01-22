
#ifndef ACCESS_POINT_FACTORY_LINUX_TEST_HXX
#define ACCESS_POINT_FACTORY_LINUX_TEST_HXX

#include <microsoft/net/wifi/AccessPointLinux.hxx>

namespace Microsoft::Net::Wifi::Test
{
struct AccessPointFactoryLinuxTest :
    public Microsoft::Net::Wifi::AccessPointFactoryLinux
{
    AccessPointFactoryLinuxTest();
};
} // namespace Microsoft::Net::Wifi::Test

#endif // ACCESS_POINT_FACTORY_LINUX_TEST_HXX
