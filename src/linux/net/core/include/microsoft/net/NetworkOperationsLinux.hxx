
#ifndef NETWORK_OPERATIONS_LINUX_HXX
#define NETWORK_OPERATIONS_LINUX_HXX

#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

#include <microsoft/net/INetworkOperations.hxx>
#include <microsoft/net/NetworkInterface.hxx>
#include <microsoft/net/NetworkIpAddress.hxx>

namespace Microsoft::Net
{
/**
 * @brief Linux implementation of network operations.
 */
struct NetworkOperationsLinux :
    public INetworkOperations
{
    ~NetworkOperationsLinux() override = default;

    /**
     * @brief Construct a new NetworkOperationsLinux object.
     */
    NetworkOperationsLinux() = default;

    NetworkOperationsLinux(const NetworkOperationsLinux&) = delete;

    NetworkOperationsLinux(NetworkOperationsLinux&&) = delete;

    NetworkOperationsLinux&
    operator=(const NetworkOperationsLinux&) = delete;

    NetworkOperationsLinux&
    operator=(NetworkOperationsLinux&&) = delete;

    /**
     * @brief Enumerate all the network interfaces on the system.
     *
     * @return std::unordered_set<std::string>
     */
    std::unordered_set<NetworkInterfaceId>
    EnumerateNetworkInterfaces() const noexcept override;

    /**
     * @brief Get all the IP addresses for the specified network interface.
     *
     * @param networkInterfaceName The name of the network interface to get the addresses for.
     * @return std::unordered_set<NetworkIpAddress>
     */
    std::unordered_set<NetworkIpAddress>
    GetNetworkInterfaceAddresses(std::string_view networkInterfaceName) const noexcept override;

    /**
     * @brief Obtain information about the specified IP address. The returned map will contain the IP address as the key
     * and the information as the value. In the case of a fixed address, the returned map will have a single entry. In
     * the case of any "any" address (eg. 0.0.0.0, ::), the returned map will contain all available addresses.
     *
     * @param ipAddress The ip address to obtain information for.
     * @return std::unordered_map<std::string, IpAddressInformation>
     */
    std::unordered_map<std::string, IpAddressInformation>
    GetLocalIpAddressInformation(std::string_view ipAddress) const noexcept override;
};
} // namespace Microsoft::Net

#endif // NETWORK_OPERATIONS_LINUX_HXX
