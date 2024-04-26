
#ifndef NETWORK_OPERATIONS_LINUX_HXX
#define NETWORK_OPERATIONS_LINUX_HXX

#include <string>
#include <unordered_set>

#include <microsoft/net/INetworkOperations.hxx>

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
     * @brief Enumerate all local IP addresses on the system.
     *
     * @return std::unordered_set<std::string>
     */
    std::unordered_set<std::string>
    GetLocalIpAddresses() const noexcept override;

    /**
     * @brief Obtain information about a local IP address.
     *
     * @param ipAddress The IP address to get information about.
     * @return IpInformation
     */
    IpInformation
    GetLocalIpInformation(const std::string& ipAddress) const noexcept override;
};
} // namespace Microsoft::Net

#endif // NETWORK_OPERATIONS_LINUX_HXX
