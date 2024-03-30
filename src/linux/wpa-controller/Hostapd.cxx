
#include <cstdint>
#include <format>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
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
Hostapd::SetAuthenticationAlgorithms(std::vector<WpaAuthenticationAlgorithm> algorithms, EnforceConfigurationChange enforceConfigurationChange)
{
    using AlgorithmsUnderlyingType = std::underlying_type_t<decltype(algorithms)::value_type>;

    if (std::empty(algorithms)) {
        throw HostapdException("No WPA authentication algorithms were provided");
    }

    // Convert the authentication algorithm values to an OR'ed unsigned integer value expected by hostapd.
    std::string algorithmsPropertyValue;
    {
        AlgorithmsUnderlyingType algorithmsValue = 0;
        for (const auto algorithm : algorithms) {
            const auto algorithmValue = WpaAuthenticationAlgorithmPropertyValue(algorithm);
            if ((algorithmValue & ~WpaAuthenticationAlgorithmMask) != 0) {
                throw HostapdException(std::format("Invalid WPA authentication algorithm value '{}'", algorithmValue));
            }
            algorithmsValue |= algorithmValue;
        }

        algorithmsValue &= WpaAuthenticationAlgorithmMask;
        algorithmsPropertyValue = std::format("{}", algorithmsValue);
    }

    try {
        SetProperty(ProtocolHostapd::PropertyNameAuthenticationAlgorithms, algorithmsPropertyValue, enforceConfigurationChange);
    } catch (const HostapdException& e) {
        throw HostapdException(std::format("Failed to set authentication algorithms to '{}' ({})", algorithmsPropertyValue, e.what()));
    }
}

void
Hostapd::SetWpaSecurityProtocols(std::vector<WpaSecurityProtocol> protocols, EnforceConfigurationChange enforceConfigurationChange)
{
    if (std::empty(protocols)) {
        throw HostapdException("No WPA protocols were provided");
    }

    uint32_t protocolsToSet = 0;
    for (const auto protocol : protocols) {
        const auto protocolValue = WpaSecurityProtocolPropertyValue(protocol);
        if ((protocolValue & ~WpaSecurityProtocolMask) != 0) {
            throw HostapdException(std::format("Invalid WPA protocol value '{}'", protocolValue));
        }
        protocolsToSet |= protocolValue;
    }

    protocolsToSet &= WpaSecurityProtocolMask;

    const auto protocolsValue = std::format("{}", protocolsToSet);
    try {
        SetProperty(ProtocolHostapd::PropertyNameWpaSecurityProtocol, protocolsValue, enforceConfigurationChange);
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

void
Hostapd::SetPairwiseCipherSuites(WpaSecurityProtocol protocol, std::vector<WpaCipher> pairwiseCiphers, EnforceConfigurationChange enforceConfigurationChange)
{
    SetPairwiseCipherSuites({ { protocol, std::move(pairwiseCiphers) } }, enforceConfigurationChange);
}

void
Hostapd::SetPairwiseCipherSuites(std::unordered_map<WpaSecurityProtocol, std::vector<WpaCipher>> protocolPairwiseCipherMap, EnforceConfigurationChange enforceConfigurationChange)
{
    if (std::empty(protocolPairwiseCipherMap)) {
        throw HostapdException("No WPA cipher suites were provided");
    }

    for (const auto& [protocol, ciphers] : protocolPairwiseCipherMap) {
        if (std::empty(ciphers)) {
            throw HostapdException(std::format("No WPA cipher suites were provided for protocol '{}'", magic_enum::enum_name(protocol)));
        }

        std::string cipherPropertyValue;
        {
            std::ostringstream cipherPropertyValueBuilder{};
            for (const auto cipher : ciphers) {
                const auto cipherValue = WpaCipherPropertyValue(cipher);
                if (cipherValue == WpaCipherInvalidValue) {
                    throw HostapdException(std::format("Invalid WPA cipher suite value '{}' for protocol '{}", magic_enum::enum_name(cipher), magic_enum::enum_name(protocol)));
                }
                cipherPropertyValueBuilder << cipherValue << ' ';
            }

            cipherPropertyValue = cipherPropertyValueBuilder.str();
        }

        const auto cipherPropertyName = WpaCipherPropertyName(protocol);
        if (cipherPropertyName == ProtocolHostapd::PropertyNameInvalid) {
            throw HostapdException(std::format("Invalid WPA protocol value '{}'", magic_enum::enum_name(protocol)));
        }

        try {
            SetProperty(cipherPropertyName, cipherPropertyValue, enforceConfigurationChange);
        } catch (const HostapdException& e) {
            throw HostapdException(std::format("Failed to set hostapd 'wpa_cipher' property to '{}' for protocol '{}' ({})", cipherPropertyValue, magic_enum::enum_name(protocol), e.what()));
        }
    }
}

void
Hostapd::SetPresharedKey(WpaPreSharedKey preSharedKey, EnforceConfigurationChange enforceConfigurationChange)
{
    auto [pskPropertyName, pskPropertyValue] = WpaPreSharedKeyPropertyKeyAndValue(preSharedKey);

    try {
        SetProperty(pskPropertyName, pskPropertyValue, enforceConfigurationChange);
        if (enforceConfigurationChange == EnforceConfigurationChange::Now) {
            Reload();
        }
    } catch (const HostapdException& e) {
        throw HostapdException(std::format("Failed to set pre-shared key ({})", e.what()));
    }
}

void
Hostapd::SetSaePasswords(std::vector<SaePassword> saePasswords, EnforceConfigurationChange enforceConfigurationChange)
{
    try {
        // First set the special value which clears all saved passwords.
        SetProperty(ProtocolHostapd::PropertyNameSaePassword, ProtocolHostapd::PropertyValueSaePasswordClearAll, EnforceConfigurationChange::Defer);

        // Now set the new passwords, deferring the configuration change (if requested) until the end.
        for (const auto& saePassword : saePasswords) {
            AddSaePassword(saePassword, EnforceConfigurationChange::Defer);
        }

        // Now that all passwords are set, enforce the configuration change if requested.
        if (enforceConfigurationChange == EnforceConfigurationChange::Now) {
            Reload();
        }
    } catch (const HostapdException& e) {
        throw HostapdException(std::format("Failed to set sae passwords ({})", e.what()));
    }
}

void
Hostapd::AddSaePassword(SaePassword saePassword, EnforceConfigurationChange enforceConfigurationChange)
{
    std::string credentialValue(std::cbegin(saePassword.Credential), std::cend(saePassword.Credential));
    std::string saePasswordValue;
    {
        std::ostringstream saePasswordValueBuilder{};

        saePasswordValueBuilder << credentialValue;
        if (saePassword.PasswordId.has_value()) {
            saePasswordValueBuilder << ProtocolHostapd::PropertyValueSaeKeyValueSeparator
                                    << ProtocolHostapd::PropertyValueSaePasswordKeyPasswordId << ProtocolHostapd::KeyValueDelimiter << saePassword.PasswordId.value();
        }
        if (saePassword.PeerMacAddress.has_value()) {
            saePasswordValueBuilder << ProtocolHostapd::PropertyValueSaeKeyValueSeparator
                                    << ProtocolHostapd::PropertyValueSaePasswordKeyPeerMac << ProtocolHostapd::KeyValueDelimiter << saePassword.PeerMacAddress.value();
        }
        if (saePassword.VlanId.has_value()) {
            saePasswordValueBuilder << ProtocolHostapd::PropertyValueSaeKeyValueSeparator
                                    << ProtocolHostapd::PropertyValueSaePasswordKeyVlanId << ProtocolHostapd::KeyValueDelimiter << saePassword.VlanId.value();
        }

        saePasswordValue = saePasswordValueBuilder.str();
    }

    try {
        SetProperty(ProtocolHostapd::PropertyNameSaePassword, saePasswordValue, enforceConfigurationChange);
    } catch (const HostapdException& e) {
        throw HostapdException(std::format("Failed to add sae password ({})", e.what()));
    }
}
