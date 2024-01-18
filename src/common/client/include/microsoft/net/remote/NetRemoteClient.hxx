
#ifndef NET_REMOTE_CLIENT
#define NET_REMOTE_CLIENT

#include <string_view>

namespace Microsoft::Net::Remote
{
/**
 * @brief Identifiers for commands supported by the server.
 */
enum class NetRemoteCommandId
{
    None,
    WifiEnumerateAcessPoints,
};
} // namespace Microsoft::Net::Remote

#endif // NET_REMOTE_CLIENT
