
#ifndef NETLINK_80211_WIPHY_HXX
#define NETLINK_80211_WIPHY_HXX

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <linux/netlink.h>
#include <linux/nl80211.h>
#include <microsoft/net/netlink/NetlinkMessage.hxx>
#include <microsoft/net/netlink/nl80211/Netlink80211WiphyBand.hxx>
#include <netlink/msg.h>

namespace Microsoft::Net::Netlink::Nl80211
{
/**
 * @brief Represents a netlink 802.11 wiphy. This is the interface to the physical radio and describes raw capabilities
 * of the device, irrespective of how the device is being used (eg. in userspace via hostapd, etc.).
 */
struct Nl80211Wiphy
{
    uint32_t Index;
    std::string Name;
    std::vector<uint32_t> CipherSuites;
    std::unordered_map<nl80211_band, Nl80211WiphyBand> Bands;
    std::vector<nl80211_iftype> SupportedInterfaceTypes;
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
     * @brief Creates a new Nl80211Wiphy object from the specified interface name.
     *
     * @param interfaceName The interface name to create the object from.
     * @return std::optional<Nl80211Wiphy>
     */
    static std::optional<Nl80211Wiphy>
    FromInterfaceName(std::string_view interfaceName);

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
    Nl80211Wiphy(uint32_t index, std::string_view name, std::vector<uint32_t> cipherSuites, std::unordered_map<nl80211_band, Nl80211WiphyBand> bands, std::vector<nl80211_iftype> supportedInterfaceTypes, bool supportsRoaming) noexcept;

    /**
     * @brief Creates a new Nl80211Wiphy object, using the specified function to add an identifier to the message,
     * allowing nl80211 to look up the associated interface.
     *
     * @param addWiphyIdentifier The function that will add the identifier to the message. This function *must* add one of
     * the following nl80211 attributes to the passed netlink message argument:
     *  - NL80211_ATTR_IFINDEX (uint32_t)
     *  - NL80211_ATTR_WIPHY (uint32_t)
     *  - NL80211_ATTR_WDEV (uint64_t)
     * @return std::optional<Nl80211Wiphy>
     */
    static std::optional<Nl80211Wiphy>
    FromId(std::function<void(Microsoft::Net::Netlink::NetlinkMessage&)> addWiphyIdentifier);
};

} // namespace Microsoft::Net::Netlink::Nl80211

#endif // NETLINK_80211_WIPHY_HXX
