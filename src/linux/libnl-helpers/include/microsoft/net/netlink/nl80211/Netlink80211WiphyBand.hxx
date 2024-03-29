
#ifndef NETLINK_80211_WIPHY_BAND_HXX
#define NETLINK_80211_WIPHY_BAND_HXX

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

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
    uint32_t VhtCapabilities;

    // From documentation in nl80211.h under nl80211_band_attr defintion.
    static constexpr auto VhtMcsSetNumBytes{ 32 };
    std::optional<std::array<uint8_t, VhtMcsSetNumBytes>> VhtMcsSet;

    /**
     * @brief Parses a netlink attribute into a Nl80211WiphyBand.
     *
     * @param wiphyBand The netlink attribute to parse.
     * @return std::optional<Nl80211WiphyBand>
     */
    static std::optional<Nl80211WiphyBand>
    Parse(struct nlattr* wiphyBand) noexcept;

    /**
     * @brief Convert the wiphy band to a string representation.
     *
     * @return std::string
     */
    std::string
    ToString() const;

private:
    /**
     * @brief The value to multiply the bitrate by to get the actual bitrate in Mbps.
     */
    static constexpr auto BitRateUnitMbpsMultiplier = 0.1;

private:
    /**
     * @brief Construct a new Nl80211WiphyBand object.
     *
     * @param frequencies The frequencies supported by this band.
     * @param bitRates The bitrates supported by this band.
     * @param htCapabilities The high-throughput ('HT' aka 802.11n) capabilities of this band.
     * @param vhtCapabilities The very-high-throughput ('VHT' aka 802.11ac) capabilities of this band.
     */
    Nl80211WiphyBand(std::vector<WiphyBandFrequency> frequencies, std::vector<uint32_t> bitRates, uint16_t htCapabilities, uint32_t vhtCapabilities, std::optional<std::array<uint8_t, VhtMcsSetNumBytes>> vhtMcsSet) noexcept;
};
} // namespace Microsoft::Net::Netlink::Nl80211

#endif // NETLINK_80211_WIPHY_BAND_HXX
