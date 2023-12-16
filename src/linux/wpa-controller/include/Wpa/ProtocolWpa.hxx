
#ifndef PROTOCOL_WPA_HXX
#define PROTOCOL_WPA_HXX

#include <string_view>

namespace Wpa
{
/**
 * @brief Shared/common WPA daemon protocol data and helpers.
 */
struct ProtocolWpa
{
    // The delimeter used to separate key-value pairs in the WPA control protocol.
    static constexpr auto KeyValueDelimiter = '=';

    // Command payloads.
    static constexpr auto CommandPayloadPing = "PING";
    static constexpr auto CommandPayloadEnable = "ENABLE";
    static constexpr auto CommandPayloadDisable = "DISABLE";
    static constexpr auto CommandPayloadStatus = "STATUS";
    static constexpr auto CommandPayloadTerminate = "TERMINATE";
    static constexpr auto CommandPayloadRelog = "RELOG";
    static constexpr auto CommandPayloadGet = "GET";
    static constexpr auto CommandPayloadSet = "SET";
    static constexpr auto CommandPayloadNote = "NOTE";
    static constexpr auto CommandPayloadMib = "MIB";
    static constexpr auto CommandPayloadPmksa = "PMKSA";
    static constexpr auto CommandPayloadPmksaFlush = "PMKSA_FLUSH";
    static constexpr auto CommandPayloadGetCapability = "GET_CAPABILITY";
    static constexpr auto CommandPayloadUpdateBeacon = "UPDATE_BEACON";
    static constexpr auto CommandPayloadChannelSwitch = "CHAN_SWITCH";
    static constexpr auto CommandPayloadVendor = "VENDOR";
    static constexpr auto CommandPayloadFlush = "FLUSH";
    static constexpr auto CommandPayloadInterfaces = "INTERFACES";
    static constexpr auto CommandPayloadDuplicateNetwork = "DUP_NETWORK";
    static constexpr auto CommandPayloadDriverFlags = "DRIVER_FLAGS";
    static constexpr auto CommandPayloadDriverFlags2 = "DRIVER_FLAGS2";
    static constexpr auto CommandPayloadDeauthenticate = "DEAUTHENTICATE";
    static constexpr auto CommandPayloadDisassociate = "DISASSOCIATE";
    static constexpr auto CommandPayloadAcceptAcl = "ACCEPT_ACL";
    static constexpr auto CommandPayloadDenyAcl = "DENY_ACL";
    static constexpr auto CommandPayloadAclAddMac = "ADD_MAC";
    static constexpr auto CommandPayloadAclDeleteMac = "DEL_MAC";
    static constexpr auto CommandPayloadAclShow = "SHOW";
    static constexpr auto CommandPayloadAclClear = "CLEAR";
    static constexpr auto CommandPayloadLogLevel = "LOG_LEVEL";

    // Response payloads.
    static constexpr auto ResponsePayloadOk = "OK";
    static constexpr auto ResponsePayloadFail = "FAIL";
    static constexpr auto ResponsePayloadPing = "PONG";

    /**
     * @brief Determines if a response payload indicates success.
     *
     * @param response The response payload to check.
     * @return true
     * @return false
     */
    static bool
    IsResponseOk(std::string_view response);

    /**
     * @brief Determines if a response payload indicates failure.
     *
     * @param response The response payload to check.
     * @return true
     * @return false
     */
    static bool
    IsResponseFail(std::string_view response);
};
} // namespace Wpa

#endif // PROTOCOL_WPA_HXX
