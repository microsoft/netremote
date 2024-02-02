
#include <plog/Log.h>
#include <Wpa/ProtocolHostapd.hxx>
#include <Wpa/ProtocolWpa.hxx>
#include <Wpa/WpaCommandStatus.hxx>
#include <Wpa/WpaResponseStatus.hxx>

#include "WpaParsingUtilities.hxx"

using namespace Wpa;

std::unique_ptr<WpaResponseParser>
WpaCommandStatus::CreateResponseParser(const WpaCommand* command, std::string_view responsePayload) const
{
    return std::make_unique<WpaStatusResponseParser>(command, responsePayload);
}

// clang-format off
WpaStatusResponseParser::WpaStatusResponseParser(const WpaCommand* command, std::string_view responsePayload) :
    WpaResponseParser(command, responsePayload, {
        { ProtocolHostapd::ResponseStatusPropertyKeyState, WpaValuePresence::Required },
        { ProtocolHostapd::ResponseStatusPropertyKeyIeee80211N, WpaValuePresence::Required },
        { ProtocolHostapd::ResponseStatusPropertyKeyIeee80211AC, WpaValuePresence::Required },
        { ProtocolHostapd::ResponseStatusPropertyKeyIeee80211AX, WpaValuePresence::Required },
    })
// clang-format on
{
}

std::shared_ptr<WpaResponse>
WpaStatusResponseParser::ParsePayload()
{
    using namespace Wpa::Parsing;

    const auto properties = GetProperties();
    const auto response = std::make_shared<WpaResponseStatus>();
    auto& status = response->Status;

    for (const auto& [key, value] : properties) {
        if (key == ProtocolHostapd::ResponseStatusPropertyKeyState) {
            status.State = HostapdInterfaceStateFromString(value);
        } else if (key == ProtocolHostapd::ResponseStatusPropertyKeyIeee80211N) {
            ParseInt(value, status.Ieee80211n);
        } else if (key == ProtocolHostapd::ResponseStatusPropertyKeyDisable11N) {
            ParseInt(value, status.Disable11n);
        } else if (key == ProtocolHostapd::ResponseStatusPropertyKeyIeee80211AC) {
            ParseInt(value, status.Ieee80211ac);
        } else if (key == ProtocolHostapd::ResponseStatusPropertyKeyDisableAC) {
            ParseInt(value, status.Disable11ac);
        } else if (key == ProtocolHostapd::ResponseStatusPropertyKeyIeee80211AX) {
            ParseInt(value, status.Ieee80211ax);
        } else if (key == ProtocolHostapd::ResponseStatusPropertyKeyDisableAX) {
            ParseInt(value, status.Disable11ax);
        }
    }

    if (!!status.Ieee80211n) {
        // TODO: parse attributes that are preset when this value is set.
    }

    if (!!status.Ieee80211ac) {
        // TODO: parse attributes that are preset when this value is set.
    }

    if (!!status.Ieee80211ax) {
        // TODO: parse attributes that are preset when this value is set.
    }

    return response;
}
