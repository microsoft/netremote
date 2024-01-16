
#include <array>
#include <format>
#include <sstream>

#include <magic_enum.hpp>
#include <microsoft/net/netlink/nl80211/Netlink80211WiphyBand.hxx>
#include <netlink/attr.h>
#include <netlink/genl/genl.h>
#include <plog/Log.h>

using namespace Microsoft::Net::Netlink::Nl80211;

Nl80211WiphyBand::Nl80211WiphyBand(std::vector<WiphyBandFrequency> frequencies, std::vector<uint32_t> bitRates, uint16_t htCapabilities) noexcept :
    Frequencies(std::move(frequencies)),
    Bitrates(std::move(bitRates)),
    HtCapabilities(htCapabilities)
{
}

/* static */
std::optional<Nl80211WiphyBand>
Nl80211WiphyBand::Parse(struct nlattr *wiphyBand) noexcept
{
    // Parse the attribute message.
    std::array<struct nlattr *, NL80211_BAND_ATTR_MAX + 1> wiphyBandAttributes{};
    int ret = nla_parse(std::data(wiphyBandAttributes), std::size(wiphyBandAttributes), static_cast<struct nlattr *>(nla_data(wiphyBand)), nla_len(wiphyBand), nullptr);
    if (ret < 0) {
        LOGE << std::format("Failed to parse wiphy band attributes with error {} ({})", ret, nl_geterror(ret));
        return std::nullopt;
    }

    uint16_t htCapabilities{ 0 };
    if (wiphyBandAttributes[NL80211_BAND_ATTR_HT_CAPA] != nullptr) {
        htCapabilities = nla_get_u16(wiphyBandAttributes[NL80211_BAND_ATTR_HT_CAPA]);
    }

    std::vector<WiphyBandFrequency> frequencies{};
    if (wiphyBandAttributes[NL80211_BAND_ATTR_FREQS] != nullptr) {
        struct nlattr *wiphyBandFrequency;
        int remainingBandFrequencies;
        nla_for_each_nested(wiphyBandFrequency, wiphyBandAttributes[NL80211_BAND_ATTR_FREQS], remainingBandFrequencies)
        {
            auto frequency = WiphyBandFrequency::Parse(wiphyBandFrequency);
            if (frequency.has_value()) {
                frequencies.emplace_back(std::move(frequency.value()));
            }
        }
    }

    std::vector<uint32_t> bitRates{};
    if (wiphyBandAttributes[NL80211_BAND_ATTR_RATES] != nullptr) {
        int remainingBitRates;
        struct nlattr *bitRate;
        nla_for_each_nested(bitRate, wiphyBandAttributes[NL80211_BAND_ATTR_RATES], remainingBitRates)
        {
            std::array<struct nlattr *, NL80211_BITRATE_ATTR_MAX + 1> bitRateAttributes{};
            ret = nla_parse(std::data(bitRateAttributes), std::size(bitRateAttributes), static_cast<struct nlattr *>(nla_data(bitRate)), nla_len(bitRate), nullptr);
            if (ret < 0) {
                LOGW << std::format("Failed to parse wiphy band bit rate attributes with error {} ({})", ret, nl_geterror(ret));
                return std::nullopt;
            } else if (bitRateAttributes[NL80211_BITRATE_ATTR_RATE] == nullptr) {
                continue;
            }

            auto bitRateValue = static_cast<uint32_t>(nla_get_u32(bitRateAttributes[NL80211_BITRATE_ATTR_RATE]));
            bitRates.emplace_back(bitRateValue);
        }
    }

    return Nl80211WiphyBand(std::move(frequencies), std::move(bitRates), htCapabilities);
}

std::string
Nl80211WiphyBand::ToString() const
{
    std::ostringstream ss;

    ss << "   HT Capabilities: " << std::format("0x{:04x}\n", HtCapabilities);
    ss << "   Frequencies:\n";
    for (const auto &frequency : Frequencies) {
        ss << std::format("    {}\n", frequency.ToString());
    }
    ss << "   Bitrates:\n";
    for (const auto bitRate : Bitrates) {
        ss << std::format("    {:2.1f} Mbps\n", BitRateUnitMbpsMultiplier * bitRate);
    }

    return ss.str();
}
