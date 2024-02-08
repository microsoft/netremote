
#ifndef NET_REMOTE_WIFI_API_TRACE_HXX
#define NET_REMOTE_WIFI_API_TRACE_HXX

#include <optional>
#include <source_location>
#include <string>

#include "NetRemoteApiTrace.hxx"
#include <microsoft/net/remote/protocol/NetRemoteService.grpc.pb.h>

namespace Microsoft::Net::Remote::Service::Tracing
{
/**
 * @brief Netremote Wi-Fi API function tracer. This should be used for all Wi-Fi API calls. It accepts arguments and
 * return values as optionals and/or pointers to indicate their presence.
 */
struct NetRemoteWifiApiTrace :
    public NetRemoteApiTrace
{
    /**
     * @brief Construct a new Net RemoteWifiApiTrace object.
     *
     * @param accessPointId The access point id associated with the API request, if present.
     * @param operationStatus The result status of the operation, if present.
     * @param location The source code location of the function call.
     */
    NetRemoteWifiApiTrace(std::optional<std::string> accessPointId = std::nullopt, const Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus* operationStatus = nullptr, std::source_location location = std::source_location::current());

    /**
     * @brief Destroy the NetRemoteWifiApiTrace object.
     */
    virtual ~NetRemoteWifiApiTrace();

private:
    std::optional<std::string> m_accessPointId;
    const Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus* m_operationStatus{ nullptr };

    static constexpr auto ArgNameAccessPointId{ "Access Point" };
    static constexpr auto ArgNameErrorMessage{ "Error Message" };
    static constexpr auto ArgNameStatus{ "Status" };
};
} // namespace Microsoft::Net::Remote::Service::Tracing

#endif // NET_REMOTE_WIFI_API_TRACE_HXX
