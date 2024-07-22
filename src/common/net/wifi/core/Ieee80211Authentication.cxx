
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <format>
#include <sstream>
#include <string>

#include <microsoft/net/wifi/Ieee80211Authentication.hxx>

using namespace Microsoft::Net::Wifi;

std::string
Ieee80211RsnaPsk::ToHexEncodedValue() const
{
    assert(Encoding() == Ieee80211RsnaPskEncoding::Value);

    std::string valueHexEncoded;
    {
        const auto& value = Value();
        std::ostringstream valueHexEncodedBuilder;
        std::ranges::for_each(value, [&](std::uint8_t valueByte) {
            valueHexEncodedBuilder << std::format("{:02X}", valueByte);
        });

        valueHexEncoded = valueHexEncodedBuilder.str();
    }

    return valueHexEncoded;
}
