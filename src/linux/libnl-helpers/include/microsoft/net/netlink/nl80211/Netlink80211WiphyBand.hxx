
#ifndef NETLINK_80211_WIPHY_BAND_HXX
#define NETLINK_80211_WIPHY_BAND_HXX

#include <cstdint>
#include <optional>
#include <string>

#include <linux/netlink.h>
#include <linux/nl80211.h>
#include <netlink/msg.h>

#include <microsoft/net/netlink/nl80211/Netlink80211WiphyBandFrequency.hxx>

namespace Microsoft::Net::Netlink::Nl80211
{
/**
 * @brief Represents a netlink 802.11 wiphy radio band and its properties.
 */
struct Nl80211WiphyBand
{
    std::vector<WiphyBandFrequency> Frequencies;
    std::vector<uint32_t> Bitrates;
    uint16_t HtCapabilities;

    /**
     * @brief 
     * 
     * @return std::string 
     */
    std::string
    ToString() const;

    /**
     * @brief 
     * 
     * @param wiphyBand 
     * @return std::optional<Nl80211WiphyBand> 
     */
    static std::optional<Nl80211WiphyBand>
    Parse(struct nlattr* wiphyBand) noexcept;

private:
    /**
     * @brief Construct a new Nl80211WiphyBand object.
     * 
     * @param frequencies 
     * @param bitRates 
     * @param htCapabilities 
     */
    Nl80211WiphyBand(std::vector<WiphyBandFrequency> frequencies, std::vector<uint32_t> bitRates, uint16_t htCapabilities) noexcept;
};
} // namespace Microsoft::Net::Netlink::Nl80211

#endif // NETLINK_80211_WIPHY_BAND_HXX
