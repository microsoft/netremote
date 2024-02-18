
#include <memory>
#include <string_view>
#include <utility>

#include <Wpa/ProtocolHostapd.hxx>
#include <Wpa/ProtocolWpa.hxx>
#include <Wpa/WpaCommandStatus.hxx>
#include <Wpa/WpaKeyValuePair.hxx>
#include <Wpa/WpaResponse.hxx>
#include <Wpa/WpaResponseParser.hxx>
#include <Wpa/WpaResponseStatus.hxx>
#include <plog/Log.h>

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
        { ProtocolHostapd::PropertyNameBss, WpaValuePresence::Required },
        { ProtocolHostapd::PropertyNameBssSsid, WpaValuePresence::Required },
        { ProtocolHostapd::PropertyNameBssBssid, WpaValuePresence::Required },
        { ProtocolHostapd::PropertyNameBssNumStations, WpaValuePresence::Required },
    })
// clang-format on
{
}

std::shared_ptr<WpaResponse>
WpaStatusResponseParser::ParsePayload()
{
    using namespace Wpa::Parsing;

    const auto& properties = GetProperties();
    const auto response = std::make_shared<WpaResponseStatus>();
    auto& status = response->Status;

    const auto enforceRequiredBssInfoSize = [&](const auto sizeRequired) {
        if (std::size(status.Bss) < sizeRequired) {
            status.Bss.resize(sizeRequired);
        }
    };

    for (const auto& [key, mapped] : properties) {
        const auto& [value, index] = mapped;
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
        } else if (key == ProtocolHostapd::PropertyNameBss) {
            enforceRequiredBssInfoSize(index.value() + 1);
            status.Bss[index.value()].Interface = value;
        } else if (key == ProtocolHostapd::PropertyNameBssBssid) {
            enforceRequiredBssInfoSize(index.value() + 1);
            status.Bss[index.value()].Bssid = value;
        } else if (key == ProtocolHostapd::PropertyNameBssSsid) {
            enforceRequiredBssInfoSize(index.value() + 1);
            status.Bss[index.value()].Ssid = value;
        } else if (key == ProtocolHostapd::PropertyNameBssNumStations) {
            enforceRequiredBssInfoSize(index.value() + 1);
            ParseInt(value, status.Bss[index.value()].NumStations);
        }
    }

    if (!(status.Ieee80211n == 0)) {
        // TODO: parse attributes that are present when this value is set.
    }

    if (!(status.Ieee80211ac == 0)) {
        // TODO: parse attributes that are present when this value is set.
    }

    if (!(status.Ieee80211ax == 0)) {
        // TODO: parse attributes that are present when this value is set.
    }

    return response;
}
