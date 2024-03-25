
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
#define IP_DEFAULT     "localhost"
#define PORT_DEFAULT   5047
#define PORT_SEPARATOR ":"
#define xstr(s)        str(s)
#define str(s)         #s

    static constexpr uint32_t PortDefault{ PORT_DEFAULT };
    static constexpr std::string_view PortSeparator{ PORT_SEPARATOR };
    static constexpr std::string_view IpDefault{ IP_DEFAULT };
    static constexpr std::string_view AddressDefault{ IP_DEFAULT PORT_SEPARATOR xstr(PORT_DEFAULT) };

#undef IP_DEFAULT
#undef PORT_DEFAULT
#undef PORT_SEPARATOR
#undef xstr
#undef str
};

} // namespace Microsoft::Net::Remote::Protocol

#endif // NET_REMOTE_PROTOCOL_HXX
