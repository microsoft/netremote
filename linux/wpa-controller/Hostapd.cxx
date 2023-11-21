
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
    static constexpr auto PingCommandPayload = "PING";
    static constexpr auto PingResponsePayload = "PONG";
    static constexpr WpaCommand PingCommand(PingCommandPayload);

    const auto response = m_controller.SendCommand(PingCommand);
    if (!response)
    {
        return false;
    }

    return (response->Payload.starts_with(PingResponsePayload));
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
