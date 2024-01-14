
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
 * @brief Represents data about a frequency in a netlink 802.11 wiphy radio band.
 */
struct WiphyBandFrequency
{
    uint32_t Frequency;
    std::optional<uint32_t> FrequencyOffset;
    bool IsDisabled;

    /**
     * @brief 
     * 
     * @param wiphyBandFrequency 
     * @return std::optional<WiphyBandFrequency> 
     */
    static std::optional<WiphyBandFrequency>
    Parse(struct nlattr* wiphyBandFrequency) noexcept;

    /**
     * @brief 
     * 
     * @return std::string 
     */
    std::string
    ToString() const;

private:
    /**
     * @brief Construct a new Wiphy Band Frequency object
     * 
     * @param frequency 
     * @param frequencyOffset 
     * @param isDisabled 
     */
    WiphyBandFrequency(uint32_t frequency, std::optional<uint32_t> frequencyOffset, bool isDisabled) noexcept;
};
} // namespace Microsoft::Net::Netlink::Nl80211

#endif // NETLINK_80211_WIPHY_BAND_FREQUENCY_HXX
