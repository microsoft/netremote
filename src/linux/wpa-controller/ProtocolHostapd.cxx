
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
    } else if (std::holds_alternative<WpaPskSecretT>(wpaPreSharedKey)) {
        const auto& pskSecret = std::get<WpaPskSecretT>(wpaPreSharedKey);
        return std::string(std::data(pskSecret), std::size(pskSecret));
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
    } else if (std::holds_alternative<WpaPskSecretT>(wpaPreSharedKey)) {
        propertyName = ProtocolHostapd::PropertyNameWpaPsk;
    } else {
        propertyName = ProtocolHostapd::PropertyNameInvalid;
    }

    auto propertyValue = WpaPreSharedKeyPropertyValue(wpaPreSharedKey);
    return std::make_pair(propertyName, std::move(propertyValue));
}
