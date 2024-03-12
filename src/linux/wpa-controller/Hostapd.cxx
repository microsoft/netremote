
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

void
Hostapd::Ping()
{
    static constexpr WpaCommand PingCommand(ProtocolHostapd::CommandPayloadPing);

    const auto response = m_controller.SendCommand(PingCommand);
    if (!response) {
        throw HostapdException("Failed to ping hostapd");
    }

    if (!response->Payload().starts_with(ProtocolHostapd::ResponsePayloadPing)) {
        LOGV << std::format("Invalid response received when sending hostapd ping\nResponse payload={}", response->Payload());
        throw HostapdException("Invalid response received when pinging hostapd");
    }
}

void
Hostapd::Reload()
{
    static constexpr WpaCommand ReloadCommand(ProtocolHostapd::CommandPayloadReload);

    const auto response = m_controller.SendCommand(ReloadCommand);
    if (!response) {
        throw HostapdException("Failed to send hostapd 'reload' command");
    }

    if (!response->IsOk()) {
        LOGV << std::format("Invalid response received when reloading hostapd configuration\nResponse payload={}", response->Payload());
        throw HostapdException("Invalid response received when reloading hostapd configuration");
    }
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

std::string
Hostapd::GetProperty(std::string_view propertyName)
{
    const WpaCommandGet getCommand(propertyName);
    const auto response = m_controller.SendCommand(getCommand);
    if (!response) {
        throw HostapdException("Failed to send hostapd 'get' command");
    }

    // Check Failed() instead of IsOk() since the response will indicate failure
    // with "FAIL", otherwise, the payload is the property value (not "OK").
    if (response->Failed()) {
        LOGV << std::format("Invalid response received when getting hostapd property '{}'\nResponse payload={}", propertyName, response->Payload());
        throw HostapdException(std::format("Failed to get hostapd property '{}' (invalid response)", propertyName));
    }

    std::string propertyValue{ response->Payload() };
    return propertyValue;
}

void
Hostapd::SetProperty(std::string_view propertyName, std::string_view propertyValue, EnforceConfigurationChange enforceConfigurationChange)
{
    LOGD << std::format("Attempting to set hostapd property '{}' (size={}) to '{}' (size={})", propertyName, std::size(propertyName), propertyValue, std::size(propertyValue));

    const WpaCommandSet setCommand(propertyName, propertyValue);
    const auto response = m_controller.SendCommand(setCommand);
    if (!response) {
        throw HostapdException("Failed to send hostapd 'set' command");
    }

    if (!response->IsOk()) {
        LOGV << std::format("Invalid response received when setting hostapd property '{}' to '{}'\nResponse payload={}", propertyName, propertyValue, response->Payload());
        throw HostapdException(std::format("Failed to set hostapd property '{}' to '{}' (invalid response)", propertyName, propertyValue));
    }

    if (enforceConfigurationChange == EnforceConfigurationChange::Defer) {
        LOGD << std::format("Skipping enforcement of '{}' configuration change (requested)", propertyName);
        return;
    }

    try {
        Reload();
    } catch (HostapdException& e) {
        throw HostapdException(std::format("Failed to reload hostapd configuration after '{}' property change: {}", propertyName, e.what()));
    }
}

void
Hostapd::Enable()
{
    static constexpr WpaCommand EnableCommand(ProtocolHostapd::CommandPayloadEnable);

    const auto response = m_controller.SendCommand(EnableCommand);
    if (!response) {
        throw HostapdException("Failed to send hostapd 'enable' command");
    }

    if (response->IsOk()) {
        return;
    }

    // The response will indicate failure if the interface is already enabled.
    // Check if this is the case by validating the interface status.
    // This is only done if the 'enable' command fails since the GetStatus()
    // command is fairly heavy-weight in terms of its payload.
    const auto status = GetStatus();
    if (!IsHostapdStateOperational(status.State)) {
        throw HostapdException(std::format("Failed to enable hostapd interface (invalid state {})", magic_enum::enum_name(status.State)));
    }
}

void
Hostapd::Disable()
{
    static constexpr WpaCommand DisableCommand(ProtocolHostapd::CommandPayloadDisable);

    const auto response = m_controller.SendCommand(DisableCommand);
    if (!response) {
        throw HostapdException("Failed to send hostapd 'disable' command");
    }

    if (response->IsOk()) {
        return;
    }

    // The response will indicate failure if the interface is already disabled.
    // Check if this is the case by validating the interface status.
    // This is only done if the 'disable' command fails since the GetStatus()
    // command is fairly heavy-weight in terms of its payload.
    const auto status = GetStatus();
    if (IsHostapdStateOperational(status.State)) {
        throw HostapdException(std::format("Failed to disable hostapd interface (invalid state {})", magic_enum::enum_name(status.State)));
    }
}

void
Hostapd::Terminate()
{
    static constexpr WpaCommand TerminateCommand(ProtocolHostapd::CommandPayloadTerminate);

    const auto response = m_controller.SendCommand(TerminateCommand);
    if (!response) {
        throw HostapdException("Failed to send hostapd 'terminate' command");
    }

    if (!response->IsOk()) {
        LOGV << std::format("Invalid response received when terminating hostapd\nResponse payload={}", response->Payload());
        throw HostapdException("Failed to terminate hostapd process (invalid response)");
    }
}

void
Hostapd::SetSsid(std::string_view ssid, EnforceConfigurationChange enforceConfigurationChange)
{
    try {
        SetProperty(ProtocolHostapd::PropertyNameSsid, ssid, enforceConfigurationChange);
    } catch (const HostapdException& e) {
        throw HostapdException(std::format("Failed to set hostapd ssid to '{}' ({})", ssid, e.what()));
    }
}

void
Hostapd::SetWpaProtocols(std::vector<WpaProtocol> protocols, EnforceConfigurationChange enforceConfigurationChange)
{
    if (std::empty(protocols)) {
        throw HostapdException("No WPA protocols were provided");
    }

    uint32_t protocolsToSet = 0;
    for (const auto protocol : protocols) {
        const auto protocolValue = std::to_underlying(protocol);
        if ((protocolValue & ~WpaProtocolMask) != 0) {
            throw HostapdException(std::format("Invalid WPA protocol value '{}'", protocolValue));
        }
        protocolsToSet |= protocolValue;
    }

    protocolsToSet &= WpaProtocolMask;

    const auto protocolsValue = std::format("{}", protocolsToSet);
    try {
        SetProperty(ProtocolHostapd::PropertyNameWpaProtocol, protocolsValue, enforceConfigurationChange);
    } catch (HostapdException& e) {
        throw HostapdException(std::format("Failed to set hostapd 'wpa' property to '{}' ({})", protocolsValue, e.what()));
    }
}

void
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
            if (keyManagementValue == WpaKeyManagementInvalidValue) {
                throw HostapdException(std::format("Invalid WPA key management value '{}'", magic_enum::enum_name(keyManagement)));
            }
            keyManagementPropertyValueBuilder << keyManagementValue << ' ';
        }

        keyManagementPropertyValue = keyManagementPropertyValueBuilder.str();
    }

    try {
        SetProperty(ProtocolHostapd::PropertyNameWpaKeyManagement, keyManagementPropertyValue, enforceConfigurationChange);
    } catch (const HostapdException& e) {
        throw HostapdException(std::format("Failed to set hostapd 'wpa_key_mgmt' property to '{}' ({})", keyManagementPropertyValue, e.what()));
    }
}
