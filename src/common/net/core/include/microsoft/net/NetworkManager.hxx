
#ifndef NETWORK_MANAGER_HXX
#define NETWORK_MANAGER_HXX

#include <memory>
#include <string>
#include <unordered_map>

#include <microsoft/net/INetworkOperations.hxx>
#include <microsoft/net/wifi/AccessPointManager.hxx>

namespace Microsoft::Net
{
/**
 * @brief Interface for managing networks on the system.
 */
struct NetworkManager
{
    /**
     * @brief Construct a new NetworkManager object with the specified network operations and access point manager.
     * 
     * @param networkOperations The network operations to use.
     * @param accessPointManager The access point manager to use.
     */
    NetworkManager(std::unique_ptr<Microsoft::Net::INetworkOperations> networkOperations, std::shared_ptr<Microsoft::Net::Wifi::AccessPointManager> accessPointManager) noexcept;

    /**
     * @brief Get the access point manager.
     * 
     * @return std::shared_ptr<Microsoft::Net::Wifi::AccessPointManager> 
     */
    std::shared_ptr<Microsoft::Net::Wifi::AccessPointManager>
    GetAccessPointManager() const noexcept;

    /**
     * @brief Get information about all network interfaces on the system.
     * 
     * @return std::unordered_map<Microsoft::Net::NetworkInterfaceId, std::unordered_set<Microsoft::Net::NetworkIpAddress>> 
     */
    std::unordered_map<Microsoft::Net::NetworkInterfaceId, std::unordered_set<Microsoft::Net::NetworkIpAddress>>
    GetNetworkInterfaceInformation() const noexcept;

    /**
     * @brief Obtain information about the specified IP address. The returned map will contain the IP address as the key
     * and the information as the value. In the case of a fixed address, the returned map will have a single entry. In
     * the case of any "any" address (eg. 0.0.0.0, ::, [::]), the returned map will contain all available addresses.
     *
     * @param ipAddress The ip address to obtain information for.
     * @return std::unordered_map<std::string, IpAddressInformation>
     */
    std::unordered_map<std::string, IpAddressInformation>
    GetLocalIpAddressInformation(std::string_view ipAddress) const noexcept;

private:
    std::shared_ptr<Microsoft::Net::INetworkOperations> m_networkOperations;
    std::shared_ptr<Microsoft::Net::Wifi::AccessPointManager> m_accessPointManager;
};
} // namespace Microsoft::Net

#endif // NETWORK_MANAGER_HXX
