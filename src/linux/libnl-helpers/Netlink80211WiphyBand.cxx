
#include <format>
#include <sstream>

#include <magic_enum.hpp>
#include <microsoft/net/netlink/nl80211/Netlink80211WiphyBand.hxx>
#include <netlink/attr.h>
#include <netlink/genl/genl.h>
#include <plog/Log.h>

using namespace Microsoft::Net::Netlink::Nl80211;

Nl80211WiphyBand::Nl80211WiphyBand(std::vector<WiphyBandFrequency> frequencies, std::vector<uint32_t> bitRates, uint16_t htCapabilities, uint32_t VhtCapabilities, std::optional<std::array<uint8_t, VhtMcsSetNumBytes>> vhtMcsSetOpt) noexcept :
    Frequencies(std::move(frequencies)),
    Bitrates(std::move(bitRates)),
    HtCapabilities(htCapabilities),
    VhtCapabilities(VhtCapabilities),
    VhtMcsSet(std::move(vhtMcsSetOpt))
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

    uint32_t vhtCapabilities{ 0 };
    std::optional<std::array<uint8_t, VhtMcsSetNumBytes>> vhtMcsSetOpt{};
    if (wiphyBandAttributes[NL80211_BAND_ATTR_VHT_CAPA] != nullptr) {
        vhtCapabilities = nla_get_u32(wiphyBandAttributes[NL80211_BAND_ATTR_VHT_CAPA]);
        if (wiphyBandAttributes[NL80211_BAND_ATTR_VHT_MCS_SET] != nullptr) {
            vhtMcsSetOpt.emplace();
            auto& vhtMcsSet = vhtMcsSetOpt.value();
            std::copy_n(static_cast<uint8_t *>(nla_data(wiphyBandAttributes[NL80211_BAND_ATTR_VHT_MCS_SET])), std::size(vhtMcsSet), std::begin(vhtMcsSet));
        }
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

    return Nl80211WiphyBand(std::move(frequencies), std::move(bitRates), htCapabilities, vhtCapabilities, std::move(vhtMcsSetOpt));
}

std::string
Nl80211WiphyBand::ToString() const
{
    std::ostringstream ss;

    ss << "   HT Capabilities: " << std::format("0x{:04x}\n", HtCapabilities);
    ss << "   VHT Capabilities: " << std::format("0x{:08x}\n", VhtCapabilities);
    if (VhtMcsSet.has_value()) {
        ss << "   VHT MCS Set: 0x";
        for (const auto &mcs : VhtMcsSet.value()) {
            ss << std::format("{:02x}", mcs);
        }
        ss << '\n';
    }
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
