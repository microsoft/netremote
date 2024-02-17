
#include <memory>
#include <string_view>
#include <utility>

#include <Wpa/ProtocolHostapd.hxx>
#include <Wpa/WpaCommandStatus.hxx>
#include <Wpa/WpaKeyValuePair.hxx>
#include <Wpa/WpaResponse.hxx>
#include <Wpa/WpaResponseParser.hxx>
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
    BssInfo bssInfo{};

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
        } else if (key.starts_with(ProtocolHostapd::PropertyNameBss)) {
            bssInfo.Interface = key;
        } else if (key.starts_with(ProtocolHostapd::PropertyNameBssBssid)) {
            bssInfo.Bssid = value;
        } else if (key.starts_with(ProtocolHostapd::PropertyNameBssSsid)) {
            bssInfo.Ssid = value;
        } else if (key.starts_with(ProtocolHostapd::PropertyNameBssNumStations)) {
            ParseInt(value, bssInfo.NumStations);
            // TODO: this assumes num_sta is the last property per bss entry. This is currently true, but the code
            // should not make this assumption to protect against future changes in hostapd control interface protocol.
            // Instead, the property index should be read and when it changes, the bssInfo should be added to the
            // response and a new bssInfo should be created.
            status.Bss.push_back(std::move(bssInfo));
            bssInfo = {};
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
