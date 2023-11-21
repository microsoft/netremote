
#include <notstd/Exceptions.hxx>

#include <Wpa/Hostapd.hxx>

using namespace Wpa;

Hostapd::Hostapd(std::string_view interfaceName) :
    m_interface(interfaceName),
    m_controller(interfaceName, WpaType::Hostapd)
{
}

std::string_view Hostapd::GetInterface()
{
    return m_interface;
}

bool Hostapd::Ping()
{
    throw notstd::NotImplementedException();
}

bool Hostapd::IsEnabled()
{
    // TODO: mutual exclusion
    return m_isEnabled;
}

bool Hostapd::Enable()
{
    throw notstd::NotImplementedException();
}

bool Hostapd::Disable() 
{
    throw notstd::NotImplementedException();
}