
#include <format>
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
        throw HostapdException("Failed to ping hostapd");
    }

    return response->Payload.starts_with(ProtocolHostapd::ResponsePayloadPing);
}

HostapdStatus Hostapd::GetStatus()
{
    static constexpr WpaCommand StatusCommand(ProtocolHostapd::CommandPayloadStatus);

    auto response = m_controller.SendCommand(StatusCommand);
    if (!response)
    {
        throw HostapdException("Failed to send hostapd 'status' command");
    }

    HostapdStatus hostapdStatus{};

    // Parse the response for the state field. Note that this code will later be
    // replaced by proper generic response parsing code, so this is quick-and-dirty.
    static constexpr std::string_view StateKey = "state=";

    // Find interface state string in payload.
    auto keyPosition = response->Payload.find(StateKey);
    if (keyPosition == response->Payload.npos)
    {
        // The response should always have this field and not much can be done without it.
        throw HostapdException("hostapd 'status' command response missing state field");
    }

    // Convert value string to corresponding enum value.
    const auto valuePosition = keyPosition + std::size(StateKey);
    const auto* value = std::data(response->Payload) + valuePosition;
    hostapdStatus.State = HostapdInterfaceStateFromString(value);

    return hostapdStatus;
}

bool Hostapd::IsEnabled()
{
    auto hostapdStatus = GetStatus();
    return IsHostapdStateOperational(hostapdStatus.State);
}

bool Hostapd::Enable()
{
    static constexpr WpaCommand EnableCommand(ProtocolHostapd::CommandPayloadEnable);

    const auto response = m_controller.SendCommand(EnableCommand);
    if (!response)
    {
        throw HostapdException("Failed to send hostapd 'enable' command");
    }
    else if (response->IsOk())
    {
        return true;
    }

    // The response will indicate failure if the interface is already enabled.
    // Check if this is the case by validating the interface status.
    // This is only done if the 'enable' command fails since the GetStatus()
    // command is fairly heavy-weight in terms of its payload.
    const auto status = GetStatus();
    return IsHostapdStateOperational(status.State);
}

bool Hostapd::Disable() 
{
    static constexpr WpaCommand DisableCommand(ProtocolHostapd::CommandPayloadDisable);

    const auto response = m_controller.SendCommand(DisableCommand);
    if (!response)
    {
        throw HostapdException("Failed to send hostapd 'disable' command");
    }
    else if (response->IsOk())
    {
        return true;
    }

    // The response will indicate failure if the interface is already disabled.
    // Check if this is the case by validating the interface status.
    // This is only done if the 'disable' command fails since the GetStatus()
    // command is fairly heavy-weight in terms of its payload.
    const auto status = GetStatus();
    return !IsHostapdStateOperational(status.State);
}

bool Hostapd::Terminate()
{
    static constexpr WpaCommand TerminateCommand(ProtocolHostapd::CommandPayloadTerminate);

    const auto response = m_controller.SendCommand(TerminateCommand);
    if (!response)
    {
        throw HostapdException("Failed to send hostapd 'terminate' command");
    }

    return response->IsOk();
}
