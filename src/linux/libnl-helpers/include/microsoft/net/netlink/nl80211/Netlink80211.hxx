
#ifndef NETLINK_82011_HXX
#define NETLINK_82011_HXX

#include <string_view>

#include <linux/nl80211.h>

namespace Microsoft::Net::Netlink::Nl80211
{
/**
 * @brief Convert an nl80211_commands enum value to a string.
 * 
 * @param command The nl80211_commands enum value to convert.
 * @return std::string_view The string representation of the enum value.
 */
std::string_view
Nl80211CommandToString(nl80211_commands command) noexcept;

/**
 * @brief Convert an nl80211_iftype enum value to a string.
 * 
 * @param type The nl80211_iftype enum value to convert.
 * @return std::string_view The string representation of the enum value.
 */
std::string_view
nL80211InterfaceTypeToString(nl80211_iftype type) noexcept;

} // namespace Microsoft::Net::Netlink::80211

#endif // NETLINK_82011_HXX
