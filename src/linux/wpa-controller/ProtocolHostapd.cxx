
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <variant>

#include <Wpa/ProtocolHostapd.hxx>

using namespace Wpa;

HostapdInterfaceState
Wpa::HostapdInterfaceStateFromString(std::string_view state) noexcept
{
    // NOLINTBEGIN(readability-else-after-return)
    // Implementation uses starts_with() instead of equals() to accommodate
    // unparsed payloads from command responses.
    if (state.starts_with(ProtocolHostapd::ResponsePayloadStatusUninitialized)) {
        return HostapdInterfaceState::Uninitialized;
    } else if (state.starts_with(ProtocolHostapd::ResponsePayloadStatusDisabled)) {
        return HostapdInterfaceState::Disabled;
    } else if (state.starts_with(ProtocolHostapd::ResponsePayloadStatusEnabled)) {
        return HostapdInterfaceState::Enabled;
    } else if (state.starts_with(ProtocolHostapd::ResponsePayloadStatusCountryUpdate)) {
        return HostapdInterfaceState::CountryUpdate;
    } else if (state.starts_with(ProtocolHostapd::ResponsePayloadStatusAcs)) {
        return HostapdInterfaceState::Acs;
    } else if (state.starts_with(ProtocolHostapd::ResponsePayloadStatusHtScan)) {
        return HostapdInterfaceState::HtScan;
    } else if (state.starts_with(ProtocolHostapd::ResponsePayloadStatusDfs)) {
        return HostapdInterfaceState::Dfs;
    } else if (state.starts_with(ProtocolHostapd::ResponsePayloadStatusNoIr)) {
        return HostapdInterfaceState::NoIr;
    }
    // NOLINTEND(readability-else-after-return)

    return HostapdInterfaceState::Unknown;
}

bool
Wpa::IsHostapdStateOperational(HostapdInterfaceState state) noexcept
{
    switch (state) {
    case HostapdInterfaceState::Enabled:
    case HostapdInterfaceState::CountryUpdate:
    case HostapdInterfaceState::Acs:
    case HostapdInterfaceState::HtScan:
    case HostapdInterfaceState::Dfs:
    case HostapdInterfaceState::NoIr:
        return true;
    case HostapdInterfaceState::Uninitialized:
    case HostapdInterfaceState::Disabled:
    case HostapdInterfaceState::Unknown:
    default:
        return false;
    }
}

std::string
Wpa::WpaPreSharedKeyPropertyValue(const WpaPreSharedKey& wpaPreSharedKey)
{
    if (std::holds_alternative<WpaPskPassphraseT>(wpaPreSharedKey)) {
        const auto& pskPassphrase = std::get<WpaPskPassphraseT>(wpaPreSharedKey);
        return std::string(pskPassphrase);
    } else if (std::holds_alternative<WpaPskValueT>(wpaPreSharedKey)) {
        const auto& pskValue = std::get<WpaPskValueT>(wpaPreSharedKey);
        return std::string(std::data(pskValue), std::size(pskValue));
    } else {
        return std::string();
    }
}

std::pair<std::string_view, std::string>
Wpa::WpaPreSharedKeyPropertyKeyAndValue(const WpaPreSharedKey& wpaPreSharedKey)
{
    std::string_view propertyName{};

    if (std::holds_alternative<WpaPskPassphraseT>(wpaPreSharedKey)) {
        propertyName = ProtocolHostapd::PropertyNameWpaPassphrase;
    } else if (std::holds_alternative<WpaPskValueT>(wpaPreSharedKey)) {
        propertyName = ProtocolHostapd::PropertyNameWpaPsk;
    } else {
        propertyName = ProtocolHostapd::PropertyNameInvalid;
    }

    auto propertyValue = WpaPreSharedKeyPropertyValue(wpaPreSharedKey);
    return std::make_pair(propertyName, std::move(propertyValue));
}

WpaKeyManagement
Wpa::WpaKeyManagementFromPropertyValue(std::string_view wpaKeyManagementProperty) noexcept
{
    std::string wpaKeyManagementString(wpaKeyManagementProperty);
    std::istringstream wpaKeyManagementStream(wpaKeyManagementString);
    WpaKeyManagement result{ WpaKeyManagement::Unknown };

    for (std::string wpaKeyManagement; wpaKeyManagementStream >> wpaKeyManagement;) {
        if (wpaKeyManagement == "WPA-EAP") {
            result = static_cast<WpaKeyManagement>(std::to_underlying(result) | std::to_underlying(WpaKeyManagement::Ieee8021x));
        } else if (wpaKeyManagement == "WPA-PSK") {
            result = static_cast<WpaKeyManagement>(std::to_underlying(result) | std::to_underlying(WpaKeyManagement::Psk));
        } else if (wpaKeyManagement == "FT-EAP") {
            result = static_cast<WpaKeyManagement>(std::to_underlying(result) | std::to_underlying(WpaKeyManagement::FtIeee8021x));
        } else if (wpaKeyManagement == "FT-PSK") {
            result = static_cast<WpaKeyManagement>(std::to_underlying(result) | std::to_underlying(WpaKeyManagement::FtPsk));
        } else if (wpaKeyManagement == "WPA-EAP-SHA256") {
            result = static_cast<WpaKeyManagement>(std::to_underlying(result) | std::to_underlying(WpaKeyManagement::Ieee8021xSha256));
        } else if (wpaKeyManagement == "WPA-PSK-SHA256") {
            result = static_cast<WpaKeyManagement>(std::to_underlying(result) | std::to_underlying(WpaKeyManagement::PskSha256));
        } else if (wpaKeyManagement == "SAE") {
            result = static_cast<WpaKeyManagement>(std::to_underlying(result) | std::to_underlying(WpaKeyManagement::Sae));
        } else if (wpaKeyManagement == "FT-SAE") {
            result = static_cast<WpaKeyManagement>(std::to_underlying(result) | std::to_underlying(WpaKeyManagement::FtSae));
        } else if (wpaKeyManagement == "OSEN") {
            result = static_cast<WpaKeyManagement>(std::to_underlying(result) | std::to_underlying(WpaKeyManagement::Osen));
        } else if (wpaKeyManagement == "WPA-EAP-SUITE-B") {
            result = static_cast<WpaKeyManagement>(std::to_underlying(result) | std::to_underlying(WpaKeyManagement::Ieee8021xSuiteB));
        } else if (wpaKeyManagement == "WPA-EAP-SUITE-B-192") {
            result = static_cast<WpaKeyManagement>(std::to_underlying(result) | std::to_underlying(WpaKeyManagement::Ieee8021xSuiteB192));
        } else if (wpaKeyManagement == "FILS-SHA256") {
            result = static_cast<WpaKeyManagement>(std::to_underlying(result) | std::to_underlying(WpaKeyManagement::FilsSha256));
        } else if (wpaKeyManagement == "FILS-SHA384") {
            result = static_cast<WpaKeyManagement>(std::to_underlying(result) | std::to_underlying(WpaKeyManagement::FilsSha384));
        } else if (wpaKeyManagement == "FT-FILS-SHA256") {
            result = static_cast<WpaKeyManagement>(std::to_underlying(result) | std::to_underlying(WpaKeyManagement::FtFilsSha256));
        } else if (wpaKeyManagement == "FT-FILS-SHA384") {
            result = static_cast<WpaKeyManagement>(std::to_underlying(result) | std::to_underlying(WpaKeyManagement::FtFilsSha384));
        } else if (wpaKeyManagement == "OWE") {
            result = static_cast<WpaKeyManagement>(std::to_underlying(result) | std::to_underlying(WpaKeyManagement::Owe));
        } else if (wpaKeyManagement == "DPP") {
            result = static_cast<WpaKeyManagement>(std::to_underlying(result) | std::to_underlying(WpaKeyManagement::Dpp));
        } else if (wpaKeyManagement == "FT-EAP-SHA384") {
            result = static_cast<WpaKeyManagement>(std::to_underlying(result) | std::to_underlying(WpaKeyManagement::FtIeee8021xSha384));
        } else if (wpaKeyManagement == "PASN") {
            result = static_cast<WpaKeyManagement>(std::to_underlying(result) | std::to_underlying(WpaKeyManagement::Pasn));
        }
    }

    return result;
}
