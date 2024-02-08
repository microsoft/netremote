
#include <microsoft/net/wifi/IAccessPointController.hxx>
#include <string_view>

using namespace Microsoft::Net::Wifi;

AccessPointControllerException::AccessPointControllerException(std::string_view what) :
    m_what(what)
{
}

const char*
AccessPointControllerException::what() const noexcept
{
    return m_what.c_str();
}
