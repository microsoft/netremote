
#include <Wpa/ProtocolHostapd.hxx>

using namespace Wpa;

HostapdInterfaceState Wpa::HostapdInterfaceStateFromString(std::string_view state) noexcept
{
    // Implementation uses start_with() instead of equals() to accommodate
    // unparsed payloads from command responses.
    if (state.starts_with(ProtocolHostapd::ResponsePayloadStatusUninitialized))
    {
        return HostapdInterfaceState::Uninitialized;
    }
    else if (state.starts_with(ProtocolHostapd::ResponsePayloadStatusDisabled))
    {
        return HostapdInterfaceState::Disabled;
    }
    else if (state.starts_with(ProtocolHostapd::ResponsePayloadStatusEnabled))
    {
        return HostapdInterfaceState::Enabled;
    }
    else if (state.starts_with(ProtocolHostapd::ResponsePayloadStatusCountryUpdate))
    {
        return HostapdInterfaceState::CountryUpdate;
    }
    else if (state.starts_with(ProtocolHostapd::ResponsePayloadStatusAcs))
    {
        return HostapdInterfaceState::Acs;
    }
    else if (state.starts_with(ProtocolHostapd::ResponsePayloadStatusHtScan))
    {
        return HostapdInterfaceState::HtScan;
    }
    else if (state.starts_with(ProtocolHostapd::ResponsePayloadStatusDfs))
    {
        return HostapdInterfaceState::Dfs;
    }
    else if (state.starts_with(ProtocolHostapd::ResponsePayloadStatusNoIr))
    {
        return HostapdInterfaceState::NoIr;
    }

    return HostapdInterfaceState::Unknown;
}

bool Wpa::IsHostapdStateOperational(HostapdInterfaceState state) noexcept
{
    switch (state)
    {
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
