
#ifndef NETLINK_82011_INTERFACE_HXX
#define NETLINK_82011_INTERFACE_HXX

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <linux/nl80211.h>

namespace Microsoft::Net::Netlink::Nl80211
{
struct Nl80211Interface
{
    static std::optional<Nl80211Interface>
    Parse(struct nl_msg* nl80211Message);

    static std::vector<Nl80211Interface>
    Enumerate();

    std::string
    ToString() const;

    std::string Name;
    nl80211_iftype Type;
    uint32_t Index;

private:
    Nl80211Interface() = default;
    Nl80211Interface(std::string_view name, nl80211_iftype type, uint32_t index);
};

} // namespace Microsoft::Net::Netlink::Nl80211

#endif // NETLINK_82011_INTERFACE_HXX
