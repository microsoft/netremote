
#ifndef SERVICE_API_NETWORK_ADAPTERS_HXX
#define SERVICE_API_NETWORK_ADAPTERS_HXX

#include <microsoft/net/NetworkInterface.hxx>
#include <microsoft/net/NetworkIpAddress.hxx>
#include <microsoft/net/remote/protocol/NetworkCore.pb.h>

namespace Microsoft::Net
{
/**
 * @brief Convert a network interface type to a service network interface kind.
 *
 * @param networkInterfaceType The network interface type to convert.
 * @return constexpr Microsoft::Net::NetworkInterfaceKind
 */
constexpr Microsoft::Net::NetworkInterfaceKind
ToServiceNetworkInterfaceKind(Microsoft::Net::NetworkInterfaceType networkInterfaceType) noexcept
{
    switch (networkInterfaceType) {
    case Microsoft::Net::NetworkInterfaceType::Wifi:
        return Microsoft::Net::NetworkInterfaceKindWifi;
    case Microsoft::Net::NetworkInterfaceType::Bridge:
        return Microsoft::Net::NetworkInterfaceKindBridge;
    case Microsoft::Net::NetworkInterfaceType::Other:
        return Microsoft::Net::NetworkInterfaceKindOther;
    default:
        return Microsoft::Net::NetworkInterfaceKindUnknown;
    }
}

/**
 * @brief Convert a service network interface kind to a network interface type.
 * 
 * @param networkInterfaceKind The service network interface kind to convert.
 * @return constexpr Microsoft::Net::NetworkInterfaceType 
 */
constexpr Microsoft::Net::NetworkInterfaceType
FromServiceNetworkInterfaceKind(Microsoft::Net::NetworkInterfaceKind networkInterfaceKind) noexcept
{
    switch (networkInterfaceKind) {
    case Microsoft::Net::NetworkInterfaceKindOther:
        return Microsoft::Net::NetworkInterfaceType::Other;
    case Microsoft::Net::NetworkInterfaceKindEthernet:
        return Microsoft::Net::NetworkInterfaceType::Ethernet;
    case Microsoft::Net::NetworkInterfaceKindWifi:
        return Microsoft::Net::NetworkInterfaceType::Wifi;
    case Microsoft::Net::NetworkInterfaceKindBridge:
        return Microsoft::Net::NetworkInterfaceType::Bridge;
    default:
        return Microsoft::Net::NetworkInterfaceType::Unknown;
    }
}

/**
 * @brief Convert a network IP family to a service network address family.
 *
 * @param networkIpFamily The network IP family to convert.
 * @return constexpr Microsoft::Net::NetworkAddressFamily
 */
constexpr Microsoft::Net::NetworkAddressFamily
ToServiceNetworkAddressFamily(Microsoft::Net::NetworkIpFamily networkIpFamily) noexcept
{
    switch (networkIpFamily) {
    case Microsoft::Net::NetworkIpFamily::Ipv4:
        return Microsoft::Net::NetworkAddressFamilyIpv4;
    case Microsoft::Net::NetworkIpFamily::Ipv6:
        return Microsoft::Net::NetworkAddressFamilyIpv6;
    default:
        return Microsoft::Net::NetworkAddressFamilyUnknown;
    }
}

/**
 * @brief Convert a service network address family to a network IP family.
 *
 * @param networkAddressFamily The service network address family to convert.
 * @return constexpr Microsoft::Net::NetworkIpFamily
 */
constexpr Microsoft::Net::NetworkIpFamily
FromServiceNetworkInterface(Microsoft::Net::NetworkAddressFamily networkAddressFamily) noexcept
{
    switch (networkAddressFamily) {
    case Microsoft::Net::NetworkAddressFamilyIpv4:
        return Microsoft::Net::NetworkIpFamily::Ipv4;
    case Microsoft::Net::NetworkAddressFamilyIpv6:
        return Microsoft::Net::NetworkIpFamily::Ipv6;
    default:
        return Microsoft::Net::NetworkIpFamily::Unknown;
    }
}

/**
 * @brief Convert a network IP address to a service network IP address.
 * 
 * @param networkIpAddress The network IP address to convert.
 * @return Microsoft::Net::NetworkAddress 
 */
Microsoft::Net::NetworkAddress
ToServiceNetworkAddress(const Microsoft::Net::NetworkIpAddress& networkIpAddress) noexcept;

/**
 * @brief Convert a service network IP address to a network IP address.
 * 
 * @param networkAddress The service network IP address to convert.
 * @return Microsoft::Net::NetworkIpAddress 
 */
Microsoft::Net::NetworkIpAddress
FromServiceNetworkAddress(const Microsoft::Net::NetworkAddress& networkAddress) noexcept;

/**
 * @brief Convert a service network IP address to a network IP address.
 *
 * @param networkInterfaceId The service network IP address to convert.
 * @return Microsoft::Net::NetworkInterface
 */
Microsoft::Net::NetworkInterface
ToServiceNetworkInterface(const Microsoft::Net::NetworkInterfaceId& networkInterfaceId) noexcept;

/**
 * @brief Convert a network IP address to a service network IP address.
 *
 * @param networkInterface The network IP address to convert.
 * @return Microsoft::Net::NetworkInterfaceId
 */
Microsoft::Net::NetworkInterfaceId
FromServiceNetworkInterface(const Microsoft::Net::NetworkInterface& networkInterface) noexcept;
} // namespace Microsoft::Net

#endif // SERVICE_API_NETWORK_ADAPTERS_HXX
