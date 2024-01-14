
#ifndef NETLINK_80211_WIPHY_HXX
#define NETLINK_80211_WIPHY_HXX

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <linux/netlink.h>
#include <linux/nl80211.h>
#include <netlink/msg.h>

namespace Microsoft::Net::Netlink::Nl80211
{
struct WiphyBandFrequency
{
    uint32_t Frequency;
    std::optional<uint32_t> FrequencyOffset;
    bool IsDisabled;

    static std::optional<WiphyBandFrequency>
    Parse(struct nlattr* wiphyBandFrequency) noexcept;

    std::string
    ToString() const;

private:
    WiphyBandFrequency(uint32_t frequency, std::optional<uint32_t> frequencyOffset, bool isDisabled) noexcept;
};

/**
 * @brief Represents a netlink 802.11 wiphy radio band and its properties.
 * TODO: move to own file.
 */
struct Nl80211WiphyBand
{
    std::vector<WiphyBandFrequency> Frequencies;
    std::vector<uint32_t> Bitrates;
    uint16_t HtCapabilities;

    std::string
    ToString() const;

    static std::optional<Nl80211WiphyBand>
    Parse(struct nlattr* wiphyBand) noexcept;

private:
    Nl80211WiphyBand(std::vector<WiphyBandFrequency> frequencies, std::vector<uint32_t> bitRates, uint16_t htCapabilities) noexcept;
};

/**
 * @brief Represents a netlink 802.11 wiphy.
 */
struct Nl80211Wiphy
{
    uint32_t Index;
    std::string Name;
    std::vector<uint32_t> CipherSuites;
    std::unordered_map<nl80211_band, Nl80211WiphyBand> Bands;
    bool SupportsRoaming;

    /**
     * @brief Creates a new Nl80211Wiphy object from the specified wiphy index.
     *
     * @param wiphyIndex The wiphy index to create the object from.
     * @return std::optional<Nl80211Wiphy>
     */
    static std::optional<Nl80211Wiphy>
    FromIndex(uint32_t wiphyIndex);

    /**
     * @brief Parse a netlink message into an Nl80211Wiphy. The netlink message must contain a response to the
     * NL80211_CMD_GET_WIPHY command.
     *
     * @param nl80211Message The message to parse.
     * @return std::optional<Nl80211Wiphy> A valid Nl80211Wiphy if the message was parsed successfully, otherwise has no
     * value.
     */
    static std::optional<Nl80211Wiphy>
    Parse(struct nl_msg* nl80211Message) noexcept;

    /**
     * @brief Convert the wiphy to a string representation.
     *
     * @return std::string
     */
    std::string
    ToString() const;

private:
    /**
     * @brief Construct a new Nl80211Wiphy object.
     *
     * @param index The wiphy index.
     * @param name The wiphy name.
     */
    Nl80211Wiphy(uint32_t index, std::string_view name, std::vector<uint32_t> cipherSuites, std::unordered_map<nl80211_band, Nl80211WiphyBand> bands, bool supportsRoaming) noexcept;
};

} // namespace Microsoft::Net::Netlink::Nl80211

#endif // NETLINK_80211_WIPHY_HXX
