
#include <array>
#include <cstdint>
#include <format>
#include <optional>
#include <sstream>
#include <string>

#include <linux/nl80211.h>
#include <microsoft/net/netlink/nl80211/Netlink80211WiphyBandFrequency.hxx>
#include <netlink/attr.h>
#include <netlink/errno.h>
#include <plog/Log.h>

using namespace Microsoft::Net::Netlink::Nl80211;

WiphyBandFrequency::WiphyBandFrequency(uint32_t frequency, std::optional<uint32_t> frequencyOffset, bool isDisabled) noexcept :
    Frequency(frequency),
    FrequencyOffset(frequencyOffset),
    IsDisabled(isDisabled)
{
}

/* static */
std::optional<WiphyBandFrequency>
WiphyBandFrequency::Parse(struct nlattr *wiphyBandFrequencyNlAttribute) noexcept
{
    // Parse the attribute message.
    std::array<struct nlattr *, NL80211_FREQUENCY_ATTR_MAX + 1> wiphyBandFrequenciesAttributes{};
    int ret = nla_parse(std::data(wiphyBandFrequenciesAttributes), std::size(wiphyBandFrequenciesAttributes) - 1, static_cast<struct nlattr *>(nla_data(wiphyBandFrequencyNlAttribute)), nla_len(wiphyBandFrequencyNlAttribute), nullptr);
    if (ret < 0) {
        LOGE << std::format("Failed to parse wiphy band frequency attributes with error {} ({})", ret, nl_geterror(ret));
        return std::nullopt;
    }

    if (wiphyBandFrequenciesAttributes[NL80211_FREQUENCY_ATTR_FREQ] == nullptr) {
        return std::nullopt;
    }

    const auto isDisabled = wiphyBandFrequenciesAttributes[NL80211_FREQUENCY_ATTR_DISABLED] != nullptr;
    const auto frequency = static_cast<uint32_t>(nla_get_u32(wiphyBandFrequenciesAttributes[NL80211_FREQUENCY_ATTR_FREQ]));
    std::optional<uint32_t> frequencyOffset{};
    if (wiphyBandFrequenciesAttributes[NL80211_FREQUENCY_ATTR_OFFSET] != nullptr) {
        frequencyOffset = static_cast<uint32_t>(nla_get_u32(wiphyBandFrequenciesAttributes[NL80211_FREQUENCY_ATTR_OFFSET]));
    }

    WiphyBandFrequency wiphyBandFrequency{ frequency, frequencyOffset, isDisabled };
    return wiphyBandFrequency;
}

std::string
WiphyBandFrequency::ToString() const
{
    std::ostringstream ss;
    ss << std::format("{}.{} MHz", Frequency, FrequencyOffset.value_or(0));
    if (IsDisabled) {
        ss << " (disabled)";
    }

    return ss.str();
}
