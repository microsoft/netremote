
#include <sstream>
#include <string>

#include <magic_enum.hpp>
#include <microsoft/net/wifi/Ieee80211AccessPointCapabilities.hxx>

using namespace Microsoft::Net::Wifi;

std::string
Ieee80211AccessPointCapabilities::ToString() const
{
    std::ostringstream result{};

    result << "Protocols: ";
    for (const auto& protocol : Protocols) {
        result << magic_enum::enum_name(protocol);
        result << ' ';
    }

    result << '\n'
           << "Frequency Bands: ";
    for (const auto& frequencyBand : FrequencyBands) {
        result << magic_enum::enum_name(frequencyBand);
        result << ' ';
    }

    result << '\n'
           << "Akm Suites: ";
    for (const auto& akmSuite : AkmSuites) {
        result << magic_enum::enum_name(akmSuite);
        result << ' ';
    }

    result << '\n'
           << "Cipher Suites: ";
    for (const auto& cipherSuite : CipherSuites) {
        result << magic_enum::enum_name(cipherSuite);
        result << ' ';
    }
    result << '\n';

    return result.str();
}
