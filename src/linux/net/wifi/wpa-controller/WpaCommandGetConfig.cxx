
#include <memory>
#include <string_view>
#include <utility>

#include <Wpa/ProtocolHostapd.hxx>
#include <Wpa/ProtocolWpa.hxx>
#include <Wpa/WpaCommandGetConfig.hxx>
#include <Wpa/WpaKeyValuePair.hxx>
#include <Wpa/WpaResponse.hxx>
#include <Wpa/WpaResponseGetConfig.hxx>
#include <Wpa/WpaResponseParser.hxx>
#include <plog/Log.h>

#include "WpaParsingUtilities.hxx"

using namespace Wpa;

std::unique_ptr<WpaResponseParser>
WpaCommandGetConfig::CreateResponseParser(const WpaCommand* command, std::string_view responsePayload) const
{
    return std::make_unique<WpaGetConfigResponseParser>(command, responsePayload);
}

// clang-format off
WpaGetConfigResponseParser::WpaGetConfigResponseParser(const WpaCommand* command, std::string_view responsePayload) :
    WpaResponseParser(command, responsePayload, {
        { ProtocolHostapd::ResponseGetConfigPropertyKeyBssid, WpaValuePresence::Required },
        { ProtocolHostapd::ResponseGetConfigPropertyKeySsid, WpaValuePresence::Required },
        { ProtocolHostapd::ResponseGetConfigPropertyKeyWpa, WpaValuePresence::Required },
        { ProtocolHostapd::ResponseGetConfigPropertyKeyWpaKeyMgmt, WpaValuePresence::Required },
        { ProtocolHostapd::ResponseGetConfigPropertyKeyGroupCipher, WpaValuePresence::Required },
        { ProtocolHostapd::ResponseGetConfigPropertyKeyRsnPairwiseCipher, WpaValuePresence::Required },
        { ProtocolHostapd::ResponseGetConfigPropertyKeyWpaPairwiseCipher, WpaValuePresence::Required },
    })
// clang-format on
{
}

std::shared_ptr<WpaResponse>
WpaGetConfigResponseParser::ParsePayload()
{
    using namespace Wpa::Parsing;

    const auto& properties = GetProperties();
    const auto response = std::make_shared<WpaResponseGetConfig>();
    auto& configuration = response->Configuration;

    for (const auto& [key, mapped] : properties) {
        const auto& [value, index] = mapped;
        if (key == ProtocolHostapd::ResponseGetConfigPropertyKeyBssid) {
            configuration.Bssid = value;
        } else if (key == ProtocolHostapd::ResponseGetConfigPropertyKeySsid) {
            configuration.Ssid = value;
        } else if (key == ProtocolHostapd::ResponseGetConfigPropertyKeyWpa) {
            int wpa;
            ParseInt(value, wpa);
            configuration.Wpa = static_cast<Wpa::WpaSecurityProtocol>(wpa);
        } else if (key == ProtocolHostapd::ResponseGetConfigPropertyKeyWpaKeyMgmt) {
            configuration.WpaKeyMgmt = WpaKeyManagementsFromPropertyValue(value);
        } else if (key == ProtocolHostapd::ResponseGetConfigPropertyKeyGroupCipher) {
            configuration.GroupCipher = WpaCipherFromPropertyValue(value);
        } else if (key == ProtocolHostapd::ResponseGetConfigPropertyKeyRsnPairwiseCipher) {
            configuration.RsnPairwiseCipher = WpaCipherFromPropertyValue(value);
        } else if (key == ProtocolHostapd::ResponseGetConfigPropertyKeyWpaPairwiseCipher) {
            configuration.WpaPairwiseCipher = WpaCipherFromPropertyValue(value);
        }
    }

    return response;
}
