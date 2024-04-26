
#ifndef NETWORK_OPERATIONS_HXX
#define NETWORK_OPERATIONS_HXX

#include <string>
#include <unordered_set>

namespace Microsoft::Net
{
constexpr auto Ipv4AnyAddress{ "0.0.0.0" };

/**
 * @brief The type of network interface.
 */
enum class NetworkInterfaceType {
    Unknown,
    Wifi,
    Other,
};

/**
 * @brief The IP family of an IP address.
 */
enum class IpFamily {
    Unknown,
    Ipv4,
    Ipv6,
};

/**
 * @brief Information about an IP address needed by the server.
 */
struct IpInformation
{
    IpFamily Family{ IpFamily::Unknown };
    NetworkInterfaceType InterfaceType{ NetworkInterfaceType::Unknown };
    bool IsRoutable{ false };
};

/**
 * @brief Interface for network operations needed by the server.
 */
struct INetworkOperations
{
    virtual ~INetworkOperations() = default;

    /**
     * @brief Enumerate all local IP addresses on the system.
     *
     * @return std::unordered_set<std::string>
     */
    virtual std::unordered_set<std::string>
    GetLocalIpAddresses() const noexcept = 0;

    /**
     * @brief Obtain information about a local IP address.
     *
     * @param ipAddress The IP address to get information about.
     * @return IpInformation
     */
    virtual IpInformation
    GetLocalIpInformation(const std::string& ipAddress) const noexcept = 0;
};
} // namespace Microsoft::Net

#endif // NETWORK_OPERATIONS_HXX
