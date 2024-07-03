
#include <iterator>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <variant>

#include <Wpa/ProtocolHostapd.hxx>
#include <strings/StringHelpers.hxx>

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
    // String values used here were from hostapd config_file.c::hostapd_config_parse_key_mgmt().
    // https://w1.fi/cgit/hostap/tree/hostapd/config_file.c?h=hostap_2_10#n682

    // NOLINTBEGIN(readability-else-after-return)
    if (wpaKeyManagementProperty == "WPA-EAP") {
        return WpaKeyManagement::Ieee8021x;
    } else if (wpaKeyManagementProperty == "WPA-PSK") {
        return WpaKeyManagement::Psk;
    } else if (wpaKeyManagementProperty == "FT-EAP") {
        return WpaKeyManagement::FtIeee8021x;
    } else if (wpaKeyManagementProperty == "FT-PSK") {
        return WpaKeyManagement::FtPsk;
    } else if (wpaKeyManagementProperty == "WPA-EAP-SHA256") {
        return WpaKeyManagement::Ieee8021xSha256;
    } else if (wpaKeyManagementProperty == "WPA-PSK-SHA256") {
        return WpaKeyManagement::PskSha256;
    } else if (wpaKeyManagementProperty == "SAE") {
        return WpaKeyManagement::Sae;
    } else if (wpaKeyManagementProperty == "FT-SAE") {
        return WpaKeyManagement::FtSae;
    } else if (wpaKeyManagementProperty == "OSEN") {
        return WpaKeyManagement::Osen;
    } else if (wpaKeyManagementProperty == "WPA-EAP-SUITE-B") {
        return WpaKeyManagement::Ieee8021xSuiteB;
    } else if (wpaKeyManagementProperty == "WPA-EAP-SUITE-B-192") {
        return WpaKeyManagement::Ieee8021xSuiteB192;
    } else if (wpaKeyManagementProperty == "FILS-SHA256") {
        return WpaKeyManagement::FilsSha256;
    } else if (wpaKeyManagementProperty == "FILS-SHA384") {
        return WpaKeyManagement::FilsSha384;
    } else if (wpaKeyManagementProperty == "FT-FILS-SHA256") {
        return WpaKeyManagement::FtFilsSha256;
    } else if (wpaKeyManagementProperty == "FT-FILS-SHA384") {
        return WpaKeyManagement::FtFilsSha384;
    } else if (wpaKeyManagementProperty == "OWE") {
        return WpaKeyManagement::Owe;
    } else if (wpaKeyManagementProperty == "DPP") {
        return WpaKeyManagement::Dpp;
    } else if (wpaKeyManagementProperty == "FT-EAP-SHA384") {
        return WpaKeyManagement::FtIeee8021xSha384;
    } else if (wpaKeyManagementProperty == "PASN") {
        return WpaKeyManagement::Pasn;
    } else {
        return WpaKeyManagement::Unknown;
    }
    // NOLINTEND(readability-else-after-return)
}

std::vector<WpaKeyManagement>
Wpa::WpaKeyManagementsFromPropertyValue(std::string_view wpaKeyManagementProperty) noexcept
{
    auto wpaKeyManagementStrings = std::views::split(wpaKeyManagementProperty, ' ') | std::views::transform(Strings::ToStringView) | std::views::transform(WpaKeyManagementFromPropertyValue);

    std::vector<WpaKeyManagement> wpaKeyManagements{
        std::make_move_iterator(std::begin(wpaKeyManagementStrings)),
        std::make_move_iterator(std::end(wpaKeyManagementStrings))
    };

    return wpaKeyManagements;
}
