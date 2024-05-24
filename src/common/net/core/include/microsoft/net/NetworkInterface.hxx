
#ifndef NETWORK_INTERFACE_HXX
#define NETWORK_INTERFACE_HXX

#include <string>
#include <unordered_set>

#include <microsoft/net/NetworkIpAddress.hxx>

namespace Microsoft::Net
{
/**
 * @brief The type of network interface.
 */
enum class NetworkInterfaceType {
    Unknown,
    Other,
    Ethernet,
    Wifi,
    Bridge,
};

/**
 * @brief Describes a network interface identifier.
 */
struct NetworkInterfaceId
{
    std::string Name;
    NetworkInterfaceType Type{ NetworkInterfaceType::Unknown };

    auto
    operator<=>(const NetworkInterfaceId&) const = default;
};
} // namespace Microsoft::Net

namespace std
{
template <>
struct hash<Microsoft::Net::NetworkInterfaceId>
{
    std::size_t
    operator()(const Microsoft::Net::NetworkInterfaceId& networkInterfaceId) const noexcept
    {
        return std::hash<std::string>{}(networkInterfaceId.Name);
    }
};
} // namespace std

#endif // NETWORK_INTERFACE_HXX
