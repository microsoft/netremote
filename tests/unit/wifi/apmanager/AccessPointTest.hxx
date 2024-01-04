
#ifndef ACCESS_POINT_TEST_HXX
#define ACCESS_POINT_TEST_HXX

#include <string>
#include <string_view>

#include <microsoft/net/wifi/IAccessPoint.hxx>

namespace Microsoft::Net::Wifi::Test
{
struct AccessPointTest final :
    Microsoft::Net::Wifi::IAccessPoint
{
    AccessPointTest(std::string_view interface) noexcept :
        m_interface{ interface }
    {
    }

    std::string_view
    GetInterface() const noexcept
    {
        return m_interface;
    }

    std::unique_ptr<IAccessPointController>
    CreateController()
    {
        // TODO
        return nullptr;
    }

    std::string m_interface;
};

} // namespace Microsoft::Net::Wifi::Test

#endif // ACCESS_POINT_TEST_HXX
