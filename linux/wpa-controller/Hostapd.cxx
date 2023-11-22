
#include <mutex>
#include <optional>

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

bool Hostapd::IsEnabled(bool forceCheck)
{
    static constexpr WpaCommand StatusCommand(ProtocolHostapd::CommandPayloadStatus);

    if (!forceCheck)
    {
        std::shared_lock stateLockShared{ m_stateGate };
        return m_isEnabled;
    }

    std::scoped_lock stateLockExclusive{ m_stateGate };
    auto response = m_controller.SendCommand(StatusCommand);
    if (!response->IsOk())
    {
        // Return false, but don't update the cached state since we didn't
        // actually get a valid response.
        // TODO: log this
        return false;
    }

    static constexpr std::string_view StateKey = "state=";

    std::optional<bool> isEnabled;
    auto keyPosition = response->Payload.find(StateKey);
    if (keyPosition == response->Payload.npos)
    {
        // TODO: the response should *always* have this field, so this is normally
        // catastrophic. For now, log and move on.
        return false;
    }

    // Obtain the value as a string.
    const auto valuePosition = keyPosition + std::size(StateKey);
    const char * const value = std::data(response->Payload) + valuePosition;

    // Attempt to parse the string value as a state response value.

    return m_isEnabled;
}

bool Hostapd::Enable()
{
    static constexpr WpaCommand EnableCommand(ProtocolHostapd::CommandPayloadEnable);

    std::scoped_lock stateLockExclusive{ m_stateGate };
    if (m_isEnabled)
    {
        return true;
    }

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
