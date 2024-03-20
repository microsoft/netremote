
#include <cstdint>
#include <format>
#include <string_view>
#include <system_error>

#include <linux/nl80211.h>
#include <magic_enum.hpp>
#include <microsoft/net/netlink/NetlinkErrorCategory.hxx>
#include <microsoft/net/netlink/NetlinkSocket.hxx>
#include <microsoft/net/netlink/nl80211/Netlink80211.hxx>
#include <netlink/genl/genl.h>
#include <plog/Log.h>

namespace Microsoft::Net::Netlink::Nl80211
{
std::string_view
Nl80211CommandToString(nl80211_commands command) noexcept
{
    switch (command) {
    case NL80211_CMD_UNSPEC:
        return "NL80211_CMD_UNSPEC";

    case NL80211_CMD_GET_WIPHY:
        return "NL80211_CMD_GET_WIPHY";
    case NL80211_CMD_SET_WIPHY:
        return "NL80211_CMD_SET_WIPHY";
    case NL80211_CMD_NEW_WIPHY:
        return "NL80211_CMD_NEW_WIPHY";
    case NL80211_CMD_DEL_WIPHY:
        return "NL80211_CMD_DEL_WIPHY";

    case NL80211_CMD_GET_INTERFACE:
        return "NL80211_CMD_GET_INTERFACE";
    case NL80211_CMD_SET_INTERFACE:
        return "NL80211_CMD_SET_INTERFACE";
    case NL80211_CMD_NEW_INTERFACE:
        return "NL80211_CMD_NEW_INTERFACE";
    case NL80211_CMD_DEL_INTERFACE:
        return "NL80211_CMD_DEL_INTERFACE";

    case NL80211_CMD_GET_KEY:
        return "NL80211_CMD_GET_KEY";
    case NL80211_CMD_SET_KEY:
        return "NL80211_CMD_SET_KEY";
    case NL80211_CMD_NEW_KEY:
        return "NL80211_CMD_NEW_KEY";
    case NL80211_CMD_DEL_KEY:
        return "NL80211_CMD_DEL_KEY";

    case NL80211_CMD_GET_BEACON:
        return "NL80211_CMD_GET_BEACON";
    case NL80211_CMD_SET_BEACON:
        return "NL80211_CMD_SET_BEACON";
    case NL80211_CMD_NEW_BEACON:
        return "NL80211_CMD_NEW_BEACON"; // NL80211_CMD_START_AP
    case NL80211_CMD_DEL_BEACON:
        return "NL80211_CMD_DEL_BEACON"; // NL80211_CMD_STOP_AP

    case NL80211_CMD_GET_STATION:
        return "NL80211_CMD_GET_STATION";
    case NL80211_CMD_SET_STATION:
        return "NL80211_CMD_SET_STATION";
    case NL80211_CMD_NEW_STATION:
        return "NL80211_CMD_NEW_STATION";
    case NL80211_CMD_DEL_STATION:
        return "NL80211_CMD_DEL_STATION";

    case NL80211_CMD_GET_MPATH:
        return "NL80211_CMD_GET_MPATH";
    case NL80211_CMD_SET_MPATH:
        return "NL80211_CMD_SET_MPATH";
    case NL80211_CMD_NEW_MPATH:
        return "NL80211_CMD_NEW_MPATH";
    case NL80211_CMD_DEL_MPATH:
        return "NL80211_CMD_DEL_MPATH";

    case NL80211_CMD_SET_BSS:
        return "NL80211_CMD_SET_BSS";

    case NL80211_CMD_SET_REG:
        return "NL80211_CMD_SET_REG";
    case NL80211_CMD_REQ_SET_REG:
        return "NL80211_CMD_REQ_SET_REG";

    case NL80211_CMD_GET_MESH_CONFIG:
        return "NL80211_CMD_GET_MESH_CONFIG";
    case NL80211_CMD_SET_MESH_CONFIG:
        return "NL80211_CMD_SET_MESH_CONFIG";

    case NL80211_CMD_SET_MGMT_EXTRA_IE:
        return "NL80211_CMD_SET_MGMT_EXTRA_IE";

    case NL80211_CMD_GET_REG:
        return "NL80211_CMD_GET_REG";

    case NL80211_CMD_GET_SCAN:
        return "NL80211_CMD_GET_SCAN";
    case NL80211_CMD_TRIGGER_SCAN:
        return "NL80211_CMD_TRIGGER_SCAN";
    case NL80211_CMD_NEW_SCAN_RESULTS:
        return "NL80211_CMD_NEW_SCAN_RESULTS";
    case NL80211_CMD_SCAN_ABORTED:
        return "NL80211_CMD_SCAN_ABORTED";

    case NL80211_CMD_REG_CHANGE:
        return "NL80211_CMD_REG_CHANGE";

    case NL80211_CMD_AUTHENTICATE:
        return "NL80211_CMD_AUTHENTICATE";
    case NL80211_CMD_ASSOCIATE:
        return "NL80211_CMD_ASSOCIATE";
    case NL80211_CMD_DEAUTHENTICATE:
        return "NL80211_CMD_DEAUTHENTICATE";
    case NL80211_CMD_DISASSOCIATE:
        return "NL80211_CMD_DISASSOCIATE";

    case NL80211_CMD_MICHAEL_MIC_FAILURE:
        return "NL80211_CMD_MICHAEL_MIC_FAILURE";

    case NL80211_CMD_REG_BEACON_HINT:
        return "NL80211_CMD_REG_BEACON_HINT";

    case NL80211_CMD_JOIN_IBSS:
        return "NL80211_CMD_JOIN_IBSS";
    case NL80211_CMD_LEAVE_IBSS:
        return "NL80211_CMD_LEAVE_IBSS";

    case NL80211_CMD_TESTMODE:
        return "NL80211_CMD_TESTMODE";

    case NL80211_CMD_CONNECT:
        return "NL80211_CMD_CONNECT";
    case NL80211_CMD_ROAM:
        return "NL80211_CMD_ROAM";
    case NL80211_CMD_DISCONNECT:
        return "NL80211_CMD_DISCONNECT";

    case NL80211_CMD_SET_WIPHY_NETNS:
        return "NL80211_CMD_SET_WIPHY_NETNS";

    case NL80211_CMD_GET_SURVEY:
        return "NL80211_CMD_GET_SURVEY";
    case NL80211_CMD_NEW_SURVEY_RESULTS:
        return "NL80211_CMD_NEW_SURVEY_RESULTS";

    case NL80211_CMD_SET_PMKSA:
        return "NL80211_CMD_SET_PMKSA";
    case NL80211_CMD_DEL_PMKSA:
        return "NL80211_CMD_DEL_PMKSA";
    case NL80211_CMD_FLUSH_PMKSA:
        return "NL80211_CMD_FLUSH_PMKSA";

    case NL80211_CMD_REMAIN_ON_CHANNEL:
        return "NL80211_CMD_REMAIN_ON_CHANNEL";
    case NL80211_CMD_CANCEL_REMAIN_ON_CHANNEL:
        return "NL80211_CMD_CANCEL_REMAIN_ON_CHANNEL";

    case NL80211_CMD_SET_TX_BITRATE_MASK:
        return "NL80211_CMD_SET_TX_BITRATE_MASK";

    case NL80211_CMD_REGISTER_ACTION:
        return "NL80211_CMD_REGISTER_ACTION"; // NL80211_CMD_REGISTER_FRAME
    case NL80211_CMD_ACTION:
        return "NL80211_CMD_ACTION"; // NL80211_CMD_FRAME
    case NL80211_CMD_ACTION_TX_STATUS:
        return "NL80211_CMD_ACTION_TX_STATUS"; // NL80211_CMD_FRAME_TX_STATUS

    case NL80211_CMD_SET_POWER_SAVE:
        return "NL80211_CMD_SET_POWER_SAVE";
    case NL80211_CMD_GET_POWER_SAVE:
        return "NL80211_CMD_GET_POWER_SAVE";

    case NL80211_CMD_SET_CQM:
        return "NL80211_CMD_SET_CQM";
    case NL80211_CMD_NOTIFY_CQM:
        return "NL80211_CMD_NOTIFY_CQM";

    case NL80211_CMD_SET_CHANNEL:
        return "NL80211_CMD_SET_CHANNEL";
    case NL80211_CMD_SET_WDS_PEER:
        return "NL80211_CMD_SET_WDS_PEER";

    case NL80211_CMD_FRAME_WAIT_CANCEL:
        return "NL80211_CMD_FRAME_WAIT_CANCEL";

    case NL80211_CMD_JOIN_MESH:
        return "NL80211_CMD_JOIN_MESH";
    case NL80211_CMD_LEAVE_MESH:
        return "NL80211_CMD_LEAVE_MESH";

    case NL80211_CMD_UNPROT_DEAUTHENTICATE:
        return "NL80211_CMD_UNPROT_DEAUTHENTICATE";
    case NL80211_CMD_UNPROT_DISASSOCIATE:
        return "NL80211_CMD_UNPROT_DISASSOCIATE";

    case NL80211_CMD_NEW_PEER_CANDIDATE:
        return "NL80211_CMD_NEW_PEER_CANDIDATE";

    case NL80211_CMD_GET_WOWLAN:
        return "NL80211_CMD_GET_WOWLAN";
    case NL80211_CMD_SET_WOWLAN:
        return "NL80211_CMD_SET_WOWLAN";

    case NL80211_CMD_START_SCHED_SCAN:
        return "NL80211_CMD_START_SCHED_SCAN";
    case NL80211_CMD_STOP_SCHED_SCAN:
        return "NL80211_CMD_STOP_SCHED_SCAN";
    case NL80211_CMD_SCHED_SCAN_RESULTS:
        return "NL80211_CMD_SCHED_SCAN_RESULTS";
    case NL80211_CMD_SCHED_SCAN_STOPPED:
        return "NL80211_CMD_SCHED_SCAN_STOPPED";

    case NL80211_CMD_SET_REKEY_OFFLOAD:
        return "NL80211_CMD_SET_REKEY_OFFLOAD";

    case NL80211_CMD_PMKSA_CANDIDATE:
        return "NL80211_CMD_PMKSA_CANDIDATE";

    case NL80211_CMD_TDLS_OPER:
        return "NL80211_CMD_TDLS_OPER";
    case NL80211_CMD_TDLS_MGMT:
        return "NL80211_CMD_TDLS_MGMT";

    case NL80211_CMD_UNEXPECTED_FRAME:
        return "NL80211_CMD_UNEXPECTED_FRAME";

    case NL80211_CMD_PROBE_CLIENT:
        return "NL80211_CMD_PROBE_CLIENT";

    case NL80211_CMD_REGISTER_BEACONS:
        return "NL80211_CMD_REGISTER_BEACONS";

    case NL80211_CMD_UNEXPECTED_4ADDR_FRAME:
        return "NL80211_CMD_UNEXPECTED_4ADDR_FRAME";

    case NL80211_CMD_SET_NOACK_MAP:
        return "NL80211_CMD_SET_NOACK_MAP";

    case NL80211_CMD_CH_SWITCH_NOTIFY:
        return "NL80211_CMD_CH_SWITCH_NOTIFY";

    case NL80211_CMD_START_P2P_DEVICE:
        return "NL80211_CMD_START_P2P_DEVICE";
    case NL80211_CMD_STOP_P2P_DEVICE:
        return "NL80211_CMD_STOP_P2P_DEVICE";

    case NL80211_CMD_CONN_FAILED:
        return "NL80211_CMD_CONN_FAILED";

    case NL80211_CMD_SET_MCAST_RATE:
        return "NL80211_CMD_SET_MCAST_RATE";

    case NL80211_CMD_SET_MAC_ACL:
        return "NL80211_CMD_SET_MAC_ACL";

    case NL80211_CMD_RADAR_DETECT:
        return "NL80211_CMD_RADAR_DETECT";

    case NL80211_CMD_GET_PROTOCOL_FEATURES:
        return "NL80211_CMD_GET_PROTOCOL_FEATURES";

    case NL80211_CMD_UPDATE_FT_IES:
        return "NL80211_CMD_UPDATE_FT_IES";
    case NL80211_CMD_FT_EVENT:
        return "NL80211_CMD_FT_EVENT";

    case NL80211_CMD_CRIT_PROTOCOL_START:
        return "NL80211_CMD_CRIT_PROTOCOL_START";
    case NL80211_CMD_CRIT_PROTOCOL_STOP:
        return "NL80211_CMD_CRIT_PROTOCOL_STOP";

    case NL80211_CMD_GET_COALESCE:
        return "NL80211_CMD_GET_COALESCE";
    case NL80211_CMD_SET_COALESCE:
        return "NL80211_CMD_SET_COALESCE";

    case NL80211_CMD_CHANNEL_SWITCH:
        return "NL80211_CMD_CHANNEL_SWITCH";

    case NL80211_CMD_VENDOR:
        return "NL80211_CMD_VENDOR";

    case NL80211_CMD_SET_QOS_MAP:
        return "NL80211_CMD_SET_QOS_MAP";

    case NL80211_CMD_ADD_TX_TS:
        return "NL80211_CMD_ADD_TX_TS";
    case NL80211_CMD_DEL_TX_TS:
        return "NL80211_CMD_DEL_TX_TS";

    case NL80211_CMD_GET_MPP:
        return "NL80211_CMD_GET_MPP";

    case NL80211_CMD_JOIN_OCB:
        return "NL80211_CMD_JOIN_OCB";
    case NL80211_CMD_LEAVE_OCB:
        return "NL80211_CMD_LEAVE_OCB";

    case NL80211_CMD_CH_SWITCH_STARTED_NOTIFY:
        return "NL80211_CMD_CH_SWITCH_STARTED_NOTIFY";

    case NL80211_CMD_TDLS_CHANNEL_SWITCH:
        return "NL80211_CMD_TDLS_CHANNEL_SWITCH";
    case NL80211_CMD_TDLS_CANCEL_CHANNEL_SWITCH:
        return "NL80211_CMD_TDLS_CANCEL_CHANNEL_SWITCH";

    case NL80211_CMD_WIPHY_REG_CHANGE:
        return "NL80211_CMD_WIPHY_REG_CHANGE";

    case NL80211_CMD_ABORT_SCAN:
        return "NL80211_CMD_ABORT_SCAN";

    case NL80211_CMD_START_NAN:
        return "NL80211_CMD_START_NAN";
    case NL80211_CMD_STOP_NAN:
        return "NL80211_CMD_STOP_NAN";
    case NL80211_CMD_ADD_NAN_FUNCTION:
        return "NL80211_CMD_ADD_NAN_FUNCTION";
    case NL80211_CMD_DEL_NAN_FUNCTION:
        return "NL80211_CMD_DEL_NAN_FUNCTION";
    case NL80211_CMD_CHANGE_NAN_CONFIG:
        return "NL80211_CMD_CHANGE_NAN_CONFIG";
    case NL80211_CMD_NAN_MATCH:
        return "NL80211_CMD_NAN_MATCH";

    case NL80211_CMD_SET_MULTICAST_TO_UNICAST:
        return "NL80211_CMD_SET_MULTICAST_TO_UNICAST";

    case NL80211_CMD_UPDATE_CONNECT_PARAMS:
        return "NL80211_CMD_UPDATE_CONNECT_PARAMS";

    case NL80211_CMD_SET_PMK:
        return "NL80211_CMD_SET_PMK";
    case NL80211_CMD_DEL_PMK:
        return "NL80211_CMD_DEL_PMK";

    case NL80211_CMD_PORT_AUTHORIZED:
        return "NL80211_CMD_PORT_AUTHORIZED";

    case NL80211_CMD_RELOAD_REGDB:
        return "NL80211_CMD_RELOAD_REGDB";

    case NL80211_CMD_EXTERNAL_AUTH:
        return "NL80211_CMD_EXTERNAL_AUTH";

    case NL80211_CMD_STA_OPMODE_CHANGED:
        return "NL80211_CMD_STA_OPMODE_CHANGED";

    case NL80211_CMD_CONTROL_PORT_FRAME:
        return "NL80211_CMD_CONTROL_PORT_FRAME";

    case NL80211_CMD_GET_FTM_RESPONDER_STATS:
        return "NL80211_CMD_GET_FTM_RESPONDER_STATS";

    case NL80211_CMD_PEER_MEASUREMENT_START:
        return "NL80211_CMD_PEER_MEASUREMENT_START";
    case NL80211_CMD_PEER_MEASUREMENT_RESULT:
        return "NL80211_CMD_PEER_MEASUREMENT_RESULT";
    case NL80211_CMD_PEER_MEASUREMENT_COMPLETE:
        return "NL80211_CMD_PEER_MEASUREMENT_COMPLETE";

    case NL80211_CMD_NOTIFY_RADAR:
        return "NL80211_CMD_NOTIFY_RADAR";

    case NL80211_CMD_UPDATE_OWE_INFO:
        return "NL80211_CMD_UPDATE_OWE_INFO";

    case NL80211_CMD_PROBE_MESH_LINK:
        return "NL80211_CMD_PROBE_MESH_LINK";

    case NL80211_CMD_SET_TID_CONFIG:
        return "NL80211_CMD_SET_TID_CONFIG";

    case NL80211_CMD_UNPROT_BEACON:
        return "NL80211_CMD_UNPROT_BEACON";

    case NL80211_CMD_CONTROL_PORT_FRAME_TX_STATUS:
        return "NL80211_CMD_CONTROL_PORT_FRAME_TX_STATUS";

    case NL80211_CMD_SET_SAR_SPECS:
        return "NL80211_CMD_SET_SAR_SPECS";

    case NL80211_CMD_OBSS_COLOR_COLLISION:
        return "NL80211_CMD_OBSS_COLOR_COLLISION";

    case NL80211_CMD_COLOR_CHANGE_REQUEST:
        return "NL80211_CMD_COLOR_CHANGE_REQUEST";

    case NL80211_CMD_COLOR_CHANGE_STARTED:
        return "NL80211_CMD_COLOR_CHANGE_STARTED";
    case NL80211_CMD_COLOR_CHANGE_ABORTED:
        return "NL80211_CMD_COLOR_CHANGE_ABORTED";
    case NL80211_CMD_COLOR_CHANGE_COMPLETED:
        return "NL80211_CMD_COLOR_CHANGE_COMPLETED";

    case NL80211_CMD_SET_FILS_AAD:
        return "NL80211_CMD_SET_FILS_AAD";

    case NL80211_CMD_ASSOC_COMEBACK:
        return "NL80211_CMD_ASSOC_COMEBACK";

    case NL80211_CMD_ADD_LINK:
        return "NL80211_CMD_ADD_LINK";
    case NL80211_CMD_REMOVE_LINK:
        return "NL80211_CMD_REMOVE_LINK";

    case NL80211_CMD_ADD_LINK_STA:
        return "NL80211_CMD_ADD_LINK_STA";
    case NL80211_CMD_MODIFY_LINK_STA:
        return "NL80211_CMD_MODIFY_LINK_STA";
    case NL80211_CMD_REMOVE_LINK_STA:
        return "NL80211_CMD_REMOVE_LINK_STA";

    case NL80211_CMD_SET_HW_TIMESTAMP:
        return "NL80211_CMD_SET_HW_TIMESTAMP";

    case NL80211_CMD_LINKS_REMOVED:
        return "NL80211_CMD_LINKS_REMOVED";

    default:
        return "NL80211_CMD_UNKNOWN"; // this name does not correspond to a real command
    }
}

std::string_view
Nl80211InterfaceTypeToString(nl80211_iftype interfaceType) noexcept
{
    static constexpr auto InterfaceTypePrefixLength{ std::size(std::string_view("NL80211_IFTYPE_")) };

    auto interfaceTypeName{ magic_enum::enum_name(interfaceType) };
    interfaceTypeName.remove_prefix(InterfaceTypePrefixLength);

    return interfaceTypeName;
}

std::string_view
Nl80211CipherSuiteToString(uint32_t cipherSuite) noexcept
{
    static constexpr uint32_t CipherTypeWep40 = 0x000fac01;
    static constexpr uint32_t CipherTypeWep104 = 0x000fac05;
    static constexpr uint32_t CipherTypeTkip = 0x000fac02;
    static constexpr uint32_t CipherTypeCcmp128 = 0x000fac04;
    static constexpr uint32_t CipherTypeCmac = 0x000fac06;
    static constexpr uint32_t CipherTypeGcmp128 = 0x000fac08;
    static constexpr uint32_t CipherTypeGcmp256 = 0x000fac09;
    static constexpr uint32_t CipherTypeCcmp256 = 0x000fac0a;
    static constexpr uint32_t CipherTypeGmac128 = 0x000fac0b;
    static constexpr uint32_t CipherTypeGmac256 = 0x000fac0c;
    static constexpr uint32_t CipherTypeCmac256 = 0x000fac0d;
    static constexpr uint32_t CipherTypeWpiSms4 = 0x00147201;

    switch (cipherSuite) {
    case CipherTypeWep40:
        return "WEP40";
    case CipherTypeWep104:
        return "WEP104";
    case CipherTypeTkip:
        return "TKIP";
    case CipherTypeCcmp128:
        return "CCMP-128";
    case CipherTypeCmac:
        return "CMAC";
    case CipherTypeGcmp128:
        return "GCMP-128";
    case CipherTypeGcmp256:
        return "GCMP-256";
    case CipherTypeCcmp256:
        return "CCMP-256";
    case CipherTypeGmac128:
        return "GMAC-128";
    case CipherTypeGmac256:
        return "GMAC-256";
    case CipherTypeCmac256:
        return "CMAC-256";
    case CipherTypeWpiSms4:
        return "WPI-SMS4";
    default:
        return "Uknown";
    }
}

using Microsoft::Net::Netlink::NetlinkSocket;

NetlinkSocket
CreateNl80211Socket()
{
    // Allocate a new netlink socket.
    auto netlinkSocket{ NetlinkSocket::Allocate() };

    // Connect the socket to the generic netlink family.
    const int ret = genl_connect(netlinkSocket);
    if (ret < 0) {
        const auto errorCode = MakeNetlinkErrorCode(-ret);
        const auto message = std::format("Failed to connect netlink socket for nl control with error {}", errorCode.value());
        LOGE << message;
        throw std::system_error(errorCode, message);
    }

    return netlinkSocket;
}

} // namespace Microsoft::Net::Netlink::Nl80211
