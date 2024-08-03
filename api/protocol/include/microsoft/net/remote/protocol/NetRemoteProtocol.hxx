
#ifndef NET_REMOTE_PROTOCOL_HXX
#define NET_REMOTE_PROTOCOL_HXX

#include <cstdint>
#include <ranges>
#include <string_view>

namespace Microsoft::Net::Remote::Protocol
{
/**
 * @brief Static NetRemote protocol information.
 */
struct NetRemoteProtocol
{
#define SERVICE_NAME   "netremote"
#define IP_DEFAULT     "0.0.0.0"
#define PORT_DEFAULT   5047
#define PORT_SEPARATOR ":"
#define xstr(s)        str(s)
#define str(s)         #s

    /**
     * @brief Primary service related definitions.
     */

    /**
     * @brief Default port if none specified.
     */
    static constexpr uint16_t PortDefault{ PORT_DEFAULT };

    /**
     * @brief Port separator for address parsing.
     */
    static constexpr std::string_view PortSeparator{ PORT_SEPARATOR };

    /**
     * @brief Default IP address if none specified.
     */
    static constexpr std::string_view IpDefault{ IP_DEFAULT };

    /**
     * @brief Default address if none specified; includes default port.
     */
    static constexpr std::string_view AddressDefault{ IP_DEFAULT PORT_SEPARATOR xstr(PORT_DEFAULT) };

    /**
     * @brief Discovery service related definitions.
     */

    static constexpr auto DnssdServiceName = SERVICE_NAME;

    /**
     * @brief Note that all non-TCP protocols must use "udp" as the protocol value, even if the protocol is not "udp".
     * See RFC 6763 "DNS-Based Service Discovery", section 4.1.2 "Service Names" and and section 7 "Service Names" for
     * additional details and reasoning.
     */
    static constexpr auto DnssdServiceProtocol = "_" SERVICE_NAME "._" "udp";

#undef SERVICE_NAME
#undef IP_DEFAULT
#undef PORT_DEFAULT
#undef PORT_SEPARATOR
#undef xstr
#undef str
};

} // namespace Microsoft::Net::Remote::Protocol

#endif // NET_REMOTE_PROTOCOL_HXX
