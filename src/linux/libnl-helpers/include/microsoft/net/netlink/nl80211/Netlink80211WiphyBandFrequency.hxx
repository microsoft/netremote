
#ifndef NETLINK_80211_WIPHY_BAND_FREQUENCY_HXX
#define NETLINK_80211_WIPHY_BAND_FREQUENCY_HXX

#include <cstdint>
#include <optional>
#include <string>

#include <linux/netlink.h>
#include <linux/nl80211.h>
#include <netlink/msg.h>

namespace Microsoft::Net::Netlink::Nl80211
{
/**
 * @brief Represents information about a frequency in a netlink 802.11 wiphy radio band.
 */
struct WiphyBandFrequency
{
    uint32_t Frequency;
    std::optional<uint32_t> FrequencyOffset;
    bool IsDisabled;

    /**
     * @brief Parses a netlink attribute into a WiphyBandFrequency.
     *
     * @param wiphyBandFrequency The netlink attribute to parse.
     * @return std::optional<WiphyBandFrequency>
     */
    static std::optional<WiphyBandFrequency>
    Parse(struct nlattr* wiphyBandFrequency) noexcept;

    /**
     * @brief Convert the frequency to a string representation.
     *
     * @return std::string
     */
    std::string
    ToString() const;

private:
    /**
     * @brief Construct a new Wiphy Band Frequency object.
     *
     * @param frequency The frequency in MHz.
     * @param frequencyOffset The fractional portion of the frequency, if any.
     * @param isDisabled Whether the frequency is disabled.
     */
    WiphyBandFrequency(uint32_t frequency, std::optional<uint32_t> frequencyOffset, bool isDisabled) noexcept;
};
} // namespace Microsoft::Net::Netlink::Nl80211

#endif // NETLINK_80211_WIPHY_BAND_FREQUENCY_HXX
