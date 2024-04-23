
#include <cstring>
#include <format>
#include <optional>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include <linux/netlink.h>
#include <microsoft/net/netlink/NetlinkErrorCategory.hxx>
#include <microsoft/net/netlink/NetlinkSocket.hxx>
#include <microsoft/net/netlink/route/NetlinkRoute.hxx>
#include <netinet/in.h>
#include <netlink/addr.h>
#include <netlink/cache.h>
#include <netlink/errno.h>
#include <netlink/netlink.h>
#include <netlink/object.h>
#include <netlink/route/addr.h>
#include <notstd/Scope.hxx>
#include <plog/Log.h>
#include <sys/socket.h>

namespace Microsoft::Net::Netlink
{
NetlinkSocket
CreateNlRouteSocket()
{
    // Allocate a new netlink socket.
    auto socket = NetlinkSocket::Allocate();

    // Connect the socket to the route netlink family.
    const int ret = nl_connect(socket, NETLINK_ROUTE);
    if (ret < 0) {
        const auto errorCode = MakeNetlinkErrorCode(-ret);
        const auto message = std::format("Failed to connect netlink socket for nl control with error {}", errorCode.value());
        LOGE << message;
        throw std::system_error(errorCode, message);
    }

    return socket;
}

std::optional<std::vector<std::string>>
NetlinkEnumerateIpv4Addresses()
{
    auto nlRouteSocket{ CreateNlRouteSocket() };

    struct nl_cache *ipAddressCache{ nullptr };
    int ret = rtnl_addr_alloc_cache(nlRouteSocket, &ipAddressCache);
    if (ret != 0) {
        LOGE << std::format("failed to allocate address cache with error {} ({})", ret, nl_geterror(ret));
        return std::nullopt;
    }

    auto freeNlCache = notstd::scope_exit([&ipAddressCache] {
        nl_cache_free(ipAddressCache);
    });

    std::vector<std::string> ipv4Addresses{};
    struct nl_object *nlObjectIpAddress{ nullptr };
    for (nlObjectIpAddress = nl_cache_get_first(ipAddressCache); nlObjectIpAddress != nullptr; nlObjectIpAddress = nl_cache_get_next(nlObjectIpAddress)) {
        auto *nlIpAddress = reinterpret_cast<struct rtnl_addr *>(nlObjectIpAddress); // NOLINT

        {
            const auto family = rtnl_addr_get_family(nlIpAddress);
            if (family != AF_INET) {
                continue;
            }
        }

        auto *ipv4Address = rtnl_addr_get_local(nlIpAddress);
        std::string ipv4AddressAscii(INET_ADDRSTRLEN, '\0');
        nl_addr2str(ipv4Address, std::data(ipv4AddressAscii), std::size(ipv4AddressAscii));
        ipv4AddressAscii.resize(std::strlen(std::data(ipv4AddressAscii)));
        ipv4Addresses.push_back(std::move(ipv4AddressAscii));
    }

    return ipv4Addresses;
}
} // namespace Microsoft::Net::Netlink
