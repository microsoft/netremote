
#include <stdexcept>

#include <microsoft/net/wifi/AccessPoint.hxx>

using namespace Microsoft::Net::Wifi;

AccessPoint::AccessPoint(std::string_view id) :
    m_interface(id)
{}

std::string_view
AccessPoint::GetInterface() const noexcept
{
    return m_interface;
}

std::unique_ptr<Microsoft::Net::Wifi::IAccessPointController>
AccessPoint::CreateController()
{
    throw std::runtime_error("this function must be overridden by a derived class");
}
