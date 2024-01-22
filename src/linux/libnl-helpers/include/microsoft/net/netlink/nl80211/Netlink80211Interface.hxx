
#ifndef NETLINK_82011_INTERFACE_HXX
#define NETLINK_82011_INTERFACE_HXX

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <linux/netlink.h>
#include <linux/nl80211.h>
#include <netlink/msg.h>

#include <microsoft/net/netlink/nl80211/Netlink80211Wiphy.hxx>

namespace Microsoft::Net::Netlink::Nl80211
{
/**
 * @brief Represents a netlink 802.11 interface.
 */
struct Nl80211Interface
{
    std::string Name;
    nl80211_iftype Type{ nl80211_iftype::NL80211_IFTYPE_UNSPECIFIED };
    uint32_t Index;
    uint32_t WiphyIndex;

    Nl80211Interface() = default;

    /**
     * @brief Construct a new Nl80211Interface object with the specified attributes.
     *
     * @param name The name of the interface.
     * @param type The nl80211_iftype of the interface.
     * @param index The interface index in the kernel.
     * @param wiphyIndex The phy interface index in the kernel.
     */
    Nl80211Interface(std::string_view name, nl80211_iftype type, uint32_t index, uint32_t wiphyIndex) noexcept;

    /**
     * @brief Parse a netlink message into an Nl80211Interface. The netlink message must contain a response to the
     * NL80211_CMD_GET_INTERFACE command, which is encoded as a NL80211_CMD_NEW_INTERFACE.
     *
     * @param nl80211Message The message to parse.
     * @return std::optional<Nl80211Interface> Will contain a valid Nl80211Interface if the message was parsed
     * successfully, otherwise has no value, indicating the message did not contain a valid NL80211_CMD_NEW_INTERFACE
     * response
     */
    static std::optional<Nl80211Interface>
    Parse(struct nl_msg* nl80211Message) noexcept;

    /**
     * @brief Enumerate all netlink 802.11 interfaces on the system.
     *
     * @return std::vector<Nl80211Interface>
     */
    static std::vector<Nl80211Interface>
    Enumerate();

    /**
     * @brief Get the Wiphy (PHY) object associated with this interface.
     *
     * @return std::optional<Nl80211Wiphy>
     */
    std::optional<Nl80211Wiphy>
    GetWiphy() const;

    /**
     * @brief Convert the interface to a string representation.
     *
     * @return std::string
     */
    std::string
    ToString() const;
};

} // namespace Microsoft::Net::Netlink::Nl80211

#endif // NETLINK_82011_INTERFACE_HXX
