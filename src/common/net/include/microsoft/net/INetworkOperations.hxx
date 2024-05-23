
#ifndef I_NETWORK_OPERATIONS_HXX
#define I_NETWORK_OPERATIONS_HXX

#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

#include <microsoft/net/IpAddressInformation.hxx>

namespace Microsoft::Net
{
/**
 * @brief Interface for network operations needed by the server.
 */
struct INetworkOperations
{
    virtual ~INetworkOperations() = default;

    /**
     * @brief Obtain information about the specified IP address. The returned map will contain the IP address as the key
     * and the information as the value. In the case of a fixed address, the returned map will have a single entry. In
     * the case of any "any" address (eg. 0.0.0.0, ::, [::]), the returned map will contain all available addresses.
     *
     * @param ipAddress The ip address to obtain information for.
     * @return std::unordered_map<std::string, IpAddressInformation>
     */
    virtual std::unordered_map<std::string, IpAddressInformation>
    GetLocalIpAddressInformation(std::string_view ipAddress) const noexcept = 0;
};
} // namespace Microsoft::Net

#endif // I_NETWORK_OPERATIONS_HXX
