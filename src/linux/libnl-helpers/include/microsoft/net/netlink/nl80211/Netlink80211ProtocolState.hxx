
#ifndef NETLINK_80211_PROTOCOL_STATE_HXX
#define NETLINK_80211_PROTOCOL_STATE_HXX

#include <unordered_map>

#include <microsoft/net/netlink/nl80211/Netlink80211.hxx>

namespace Microsoft::Net::Netlink::Nl80211
{
/**
 * @brief Dynamic state associated with the nl80211 netlink protocol.
 */
struct Nl80211ProtocolState
{
    /**
     * @brief Driver identifier for the nl80211 protocol.
     */
    int DriverId{ -1 };

    /**
     * @brief Map of multicast group enum values to their ids.
     */
    std::unordered_map<Nl80211MulticastGroup, int> MulticastGroupId;

    /**
     * @brief Return an instance of the nl80211 protocol state.
     *
     * @return Nl80211ProtocolState&
     */
    static Nl80211ProtocolState&
    Instance();

private:
    /**
     * @brief Construct a new Nl80211ProtocolState object.
     */
    Nl80211ProtocolState();
};

} // namespace Microsoft::Net::Netlink::Nl80211

#endif // NETLINK_80211_PROTOCOL_STATE_HXX
