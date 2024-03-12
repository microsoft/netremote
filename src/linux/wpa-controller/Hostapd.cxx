
#include <cstdint>
#include <format>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <Wpa/Hostapd.hxx>
#include <Wpa/IHostapd.hxx>
#include <Wpa/ProtocolHostapd.hxx>
#include <Wpa/WpaCommand.hxx>
#include <Wpa/WpaCommandGet.hxx>
#include <Wpa/WpaCommandSet.hxx>
#include <Wpa/WpaCommandStatus.hxx>
#include <Wpa/WpaCore.hxx>
#include <Wpa/WpaResponseStatus.hxx>
#include <magic_enum.hpp>
#include <plog/Log.h>

using namespace Wpa;

Hostapd::Hostapd(std::string_view interfaceName) :
    m_interface(interfaceName),
    m_controller(interfaceName, WpaType::Hostapd)
{
}

std::string_view
Hostapd::GetInterface()
{
    return m_interface;
}

bool
Hostapd::Ping()
{
    static constexpr WpaCommand PingCommand(ProtocolHostapd::CommandPayloadPing);

    const auto response = m_controller.SendCommand(PingCommand);
    if (!response) {
        throw HostapdException("Failed to ping hostapd");
    }

    return response->Payload().starts_with(ProtocolHostapd::ResponsePayloadPing);
}

HostapdStatus
Hostapd::GetStatus()
{
    static constexpr WpaCommandStatus StatusCommand;

    auto response = m_controller.SendCommand<WpaResponseStatus>(StatusCommand);
    if (!response) {
        throw HostapdException("Failed to send hostapd 'status' command");
    }

    return response->Status;
}

bool
Hostapd::SetSsid(std::string_view ssid, bool reload)
{
    const bool ssidWasSet = SetProperty(ProtocolHostapd::PropertyNameSsid, ssid);
    if (!ssidWasSet) {
        throw HostapdException("Failed to set hostapd 'ssid' property");
    }

    if (!reload) {
        LOGW << "Skipping hostapd reload after setting 'ssid' (requested)";
        return true;
    }

    const bool configurationReloadSucceeded = Reload();
    if (!configurationReloadSucceeded) {
        throw HostapdException("Failed to reload hostapd configuration");
    }

    return true;
}

bool
Hostapd::GetProperty(std::string_view propertyName, std::string& propertyValue)
{
    const WpaCommandGet getCommand(propertyName);
    const auto response = m_controller.SendCommand(getCommand);
    if (!response) {
        throw HostapdException("Failed to send hostapd 'get' command");
    }

    // Check Failed() instead of IsOk() since the response will indicate failure
    // with "FAIL", otherwise, the payload is the property value (not "OK").
    if (response->Failed()) {
        return false;
    }

    propertyValue = response->Payload();
    return true;
}

bool
Hostapd::SetProperty(std::string_view propertyName, std::string_view propertyValue)
{
    LOGD << std::format("Attempting to set hostapd property '{}'({}) to '{}'({})", propertyName, std::size(propertyName), propertyValue, std::size(propertyValue));

    const WpaCommandSet setCommand(propertyName, propertyValue);
    const auto response = m_controller.SendCommand(setCommand);
    if (!response) {
        throw HostapdException("Failed to send hostapd 'set' command");
    }

    return response->IsOk();
}

bool
Hostapd::Enable()
{
    static constexpr WpaCommand EnableCommand(ProtocolHostapd::CommandPayloadEnable);

    const auto response = m_controller.SendCommand(EnableCommand);
    if (!response) {
        throw HostapdException("Failed to send hostapd 'enable' command");
    }

    if (response->IsOk()) {
        return true;
    }

    // The response will indicate failure if the interface is already enabled.
    // Check if this is the case by validating the interface status.
    // This is only done if the 'enable' command fails since the GetStatus()
    // command is fairly heavy-weight in terms of its payload.
    const auto status = GetStatus();
    return IsHostapdStateOperational(status.State);
}

bool
Hostapd::Disable()
{
    static constexpr WpaCommand DisableCommand(ProtocolHostapd::CommandPayloadDisable);

    const auto response = m_controller.SendCommand(DisableCommand);
    if (!response) {
        throw HostapdException("Failed to send hostapd 'disable' command");
    }

    if (response->IsOk()) {
        return true;
    }

    // The response will indicate failure if the interface is already disabled.
    // Check if this is the case by validating the interface status.
    // This is only done if the 'disable' command fails since the GetStatus()
    // command is fairly heavy-weight in terms of its payload.
    const auto status = GetStatus();
    return !IsHostapdStateOperational(status.State);
}

bool
Hostapd::Terminate()
{
    static constexpr WpaCommand TerminateCommand(ProtocolHostapd::CommandPayloadTerminate);

    const auto response = m_controller.SendCommand(TerminateCommand);
    if (!response) {
        throw HostapdException("Failed to send hostapd 'terminate' command");
    }

    return response->IsOk();
}

bool
Hostapd::Reload()
{
    static constexpr WpaCommand ReloadCommand(ProtocolHostapd::CommandPayloadReload);

    const auto response = m_controller.SendCommand(ReloadCommand);
    if (!response) {
        throw HostapdException("Failed to send hostapd 'reload' command");
    }

    return response->IsOk();
}

bool
Hostapd::SetWpaProtocols(std::vector<WpaProtocol> protocols, EnforceConfigurationChange enforceConfigurationChange)
{
    if (std::empty(protocols)) {
        throw HostapdException("No WPA protocols were provided");
    }

    uint32_t protocolsToSet = 0;
    for (const auto protocol : protocols) {
        protocolsToSet |= std::to_underlying(protocol);
    }

    protocolsToSet &= WpaProtocolMask;

    const auto protocolsValue = std::format("{}", protocolsToSet);
    const bool protocolsWereSet = SetProperty(ProtocolHostapd::PropertyNameWpaProtocol, protocolsValue);
    if (!protocolsWereSet) {
        throw HostapdException(std::format("Failed to set hostapd 'wpa' property to '{}'", protocolsValue));
    }

    if (enforceConfigurationChange == EnforceConfigurationChange::Defer) {
        LOGW << std::format("Skipping enforcement of '{}' configuration change (requested)", ProtocolHostapd::PropertyNameWpaProtocol);
        return true;
    }

    const bool configurationReloadSucceeded = Reload();
    if (!configurationReloadSucceeded) {
        throw HostapdException("Failed to reload hostapd configuration");
    }

    return true;
}

bool
Hostapd::SetKeyManagement(std::vector<WpaKeyManagement> keyManagements, EnforceConfigurationChange enforceConfigurationChange)
{
    if (std::empty(keyManagements)) {
        throw HostapdException("No WPA key management values were provided");
    }

    // Convert the key management values to a space-delimited string of individual key management values expected by hostapd.
    std::string keyManagementPropertyValue;
    {
        std::ostringstream keyManagementPropertyValueBuilder{};
        for (const auto keyManagement : keyManagements) {
            const auto keyManagementValue = WpaKeyManagementPropertyValue(keyManagement);
            if (keyManagementValue == WpaKeyManagementValueInvalid) {
                throw HostapdException(std::format("Invalid WPA key management value '{}'", magic_enum::enum_name(keyManagement)));
            }
            keyManagementPropertyValueBuilder << keyManagementValue << ' ';
        }

        keyManagementPropertyValue = keyManagementPropertyValueBuilder.str();
    }

    const auto keyManagementWasSet = SetProperty(ProtocolHostapd::PropertyNameWpaKeyManagement, keyManagementPropertyValue);
    if (!keyManagementWasSet) {
        throw HostapdException(std::format("Failed to set hostapd 'wpa_key_mgmt' property to '{}'", keyManagementPropertyValue));
    }

    if (enforceConfigurationChange == EnforceConfigurationChange::Defer) {
        LOGW << std::format("Skipping enforcement of '{}' configuration change (requested)", ProtocolHostapd::PropertyNameWpaKeyManagement);
        return true;
    }

    const bool configurationReloadSucceeded = Reload();
    if (!configurationReloadSucceeded) {
        throw HostapdException("Failed to reload hostapd configuration");
    }

    return false;
}
