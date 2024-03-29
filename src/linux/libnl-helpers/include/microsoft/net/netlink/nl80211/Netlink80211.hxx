
#ifndef NETLINK_82011_HXX
#define NETLINK_82011_HXX

#include <cstdint>
#include <initializer_list>
#include <string_view>
#include <unordered_map>

#include <linux/nl80211.h>
#include <microsoft/net/netlink/NetlinkSocket.hxx>

namespace Microsoft::Net::Netlink::Nl80211
{
enum class Nl80211MulticastGroup {
    Configuration,
    Scan,
    Regulatory,
    Mlme,
    Nan,
};

// NOLINTBEGIN(cert-err58-cpp)
/**
 * @brief Map of multicast group enum values to names.
 */
static const std::unordered_map<Nl80211MulticastGroup, std::string_view> Nl80211MulticastGroupNames{
    { Nl80211MulticastGroup::Configuration, NL80211_MULTICAST_GROUP_CONFIG },
    { Nl80211MulticastGroup::Scan, NL80211_MULTICAST_GROUP_SCAN },
    { Nl80211MulticastGroup::Regulatory, NL80211_MULTICAST_GROUP_REG },
    { Nl80211MulticastGroup::Mlme, NL80211_MULTICAST_GROUP_MLME },
    { Nl80211MulticastGroup::Nan, NL80211_MULTICAST_GROUP_NAN },
};
// NOLINTEND(cert-err58-cpp)

/**
 * @brief List of interface types that indicate support for access point operation.
 */
constexpr std::initializer_list<nl80211_iftype> Nl80211AccessPointInterfaceTypes = {
    nl80211_iftype::NL80211_IFTYPE_AP,
    nl80211_iftype::NL80211_IFTYPE_AP_VLAN,
};

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
Nl80211InterfaceTypeToString(nl80211_iftype type) noexcept;

/**
 * @brief Convert a cipher suite value to a string.
 *
 * @return std::string_view
 */
std::string_view
Nl80211CipherSuiteToString(uint32_t cipherSuite) noexcept;

/**
 * @brief Create a netlink socket for use with Nl80211.
 *
 * This creates a netlink socket and connects it to the nl80211 generic netlink family.
 *
 * @return Microsoft::Net::Netlink::NetlinkSocket
 */
Microsoft::Net::Netlink::NetlinkSocket
CreateNl80211Socket();

} // namespace Microsoft::Net::Netlink::Nl80211

#endif // NETLINK_82011_HXX
