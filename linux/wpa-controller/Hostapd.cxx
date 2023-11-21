
#include <notstd/Exceptions.hxx>

#include <Wpa/Hostapd.hxx>
#include <Wpa/ProtocolHostapd.hxx>

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
    static constexpr WpaCommand PingCommand(ProtocolHostapd::CommandPayloadPing);

    const auto response = m_controller.SendCommand(PingCommand);
    if (!response)
    {
        return false;
    }

    return response->IsOk();
}

bool Hostapd::IsEnabled()
{
    // TODO: mutual exclusion
    return m_isEnabled;
}

bool Hostapd::Enable()
{
    static constexpr WpaCommand EnableCommand(ProtocolHostapd::CommandPayloadEnable);

    const auto response = m_controller.SendCommand(EnableCommand);
    m_isEnabled = response->IsOk();
    return m_isEnabled;
}

bool Hostapd::Disable() 
{
    static constexpr WpaCommand DisableCommand(ProtocolHostapd::CommandPayloadDisable);

    const auto response = m_controller.SendCommand(DisableCommand);
    m_isEnabled = !response->IsOk();
    return !m_isEnabled;
}

bool Hostapd::Terminate()
{
    static constexpr WpaCommand TerminateCommand(ProtocolHostapd::CommandPayloadTerminate);

    const auto response = m_controller.SendCommand(TerminateCommand);
    return response->IsOk();
}
