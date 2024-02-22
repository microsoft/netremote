
#include <format>
#include <iterator>

#include <linux/nl80211.h>
#include <microsoft/net/netlink/NetlinkException.hxx>
#include <microsoft/net/netlink/NetlinkSocket.hxx>
#include <microsoft/net/netlink/nl80211/Netlink80211.hxx>
#include <microsoft/net/netlink/nl80211/Netlink80211ProtocolState.hxx>
#include <netlink/genl/ctrl.h>
#include <netlink/genl/genl.h>

using namespace Microsoft::Net::Netlink::Nl80211;

Nl80211ProtocolState::Nl80211ProtocolState()
{
    using Microsoft::Net::Netlink::NetlinkSocket;

    // Allocate a new netlink socket.
    auto netlinkSocket{ NetlinkSocket::Allocate() };

    // Connect the socket to the generic netlink family.
    const int ret = genl_connect(netlinkSocket);
    if (ret < 0) {
        throw NetlinkException::CreateLogged(-ret, "Failed to connect netlink socket for nl control");
    }

    // Look up the nl80211 driver id.
    DriverId = genl_ctrl_resolve(netlinkSocket, NL80211_GENL_NAME);
    if (DriverId < 0) {
        throw NetlinkException::CreateLogged(-DriverId, "Failed to resolve nl80211 netlink id");
    }

    // Lookup the ids for the nl80211 multicast groups.
    for (const auto& [multicastGroup, multicastGroupName] : Nl80211MulticastGroupNames) {
        const int multicastGroupId = genl_ctrl_resolve_grp(netlinkSocket, NL80211_GENL_NAME, std::data(multicastGroupName));
        if (multicastGroupId < 0) {
            throw NetlinkException::CreateLogged(-multicastGroupId, std::format("Failed to resolve nl80211 {} multicast group id", multicastGroupName));
        }
        MulticastGroupId[multicastGroup] = multicastGroupId;
    }
}

/* static */
Nl80211ProtocolState&
Nl80211ProtocolState::Instance()
{
    static Nl80211ProtocolState protocolState;
    return protocolState;
}
