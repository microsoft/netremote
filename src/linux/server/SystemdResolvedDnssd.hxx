
#ifndef SYSTEMD_RESOLVED_DNSSD_HXX
#define SYSTEMD_RESOLVED_DNSSD_HXX

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include <microsoft/net/IpAddressInformation.hxx>

namespace Microsoft::Net::Systemd
{
struct ResolvedDnssd
{
    /**
     * @brief The name of the D-Bus service for systemd-resolved.
     */
    static constexpr auto DbusServiceName = "org.freedesktop.resolve1";

    /**
     * @brief The object path for sytemd-resolved D-Bus objects.
     */
    static constexpr auto DbusServiceObjectPath = "/org/freedesktop/resolve1";

    /**
     * @brief The interface for the D-Bus service for systemd-resolved.
     */
    static constexpr auto DbusServiceInterface = "org.freedesktop.resolve1.Manager";

    /**
     * @brief Method name to register a DNS-SD service with systemd-resolved.
     */
    static constexpr auto MethodNameReigterService = "RegisterService";

    /**
     * @brief Method name to unregister a DNS-SD service with systemd-resolved.
     */
    static constexpr auto MethodNameUnregisterService = "UnregisterService";

    /**
     * @brief Build the 'TxtText' record for a DNS-SD service.
     *
     * @param ipAddresses The ip addresses to include in the record.
     * @return std::string
     */
    static std::string
    BuildTxtTextRecord(const std::unordered_map<std::string, Microsoft::Net::IpAddressInformation>& ipAddresses);

    /**
     * @brief Build a key-value pair for the 'TxtData' record for a DNS-SD service.
     * 
     * @param ipAddress The ip address to include in the record.
     * @param ipAddressInformation The information about the ip address.
     * @return std::pair<std::string, std::vector<uint8_t>> 
     */
    static std::pair<std::string, std::vector<uint8_t>>
    BuildTxtDataRecordIpAddressKeyValuePair(std::string_view ipAddress, const Microsoft::Net::IpAddressInformation& ipAddressInformation);

    /**
     * @brief Build the 'TxtData' record for a DNS-SD service.
     *
     * @param ipAddresses The ip addresses to include in the record.
     * @return std::unordered_map<std::string, std::vector<uint8_t>> 
     */
    static std::unordered_map<std::string, std::vector<uint8_t>>
    BuildTxtDataRecord(const std::unordered_map<std::string, Microsoft::Net::IpAddressInformation>& ipAddresses);

    /**
     * @brief Register a DNS-SD service with systemd-resolved.
     *
     * @param serviceName The name of the service.
     * @param protocol The protocol of the service.
     * @param port The port of the service.
     * @param txtDataRecord The 'TxtData' record for the service.
     * @param priority The optional priority of the service.
     * @param weight The optional weight of the service.
     * @return std::string The dbus object path of the newly registered service.
     */
    static std::string
    RegisterService(std::string_view serviceName, std::string_view protocol, uint16_t port, const std::vector<std::unordered_map<std::string, std::vector<uint8_t>>>& txtDataRecord, std::optional<uint16_t> priority = std::nullopt, std::optional<uint16_t> weight = std::nullopt);

    /**
     * @brief Unregister a DNS-SD service with systemd-resolved.
     *
     * @param serviceObjectPath The dbus object path of the service to unregister.
     * @return bool
     */
    static bool
    UnregisterService(std::string_view serviceObjectPath);
};
} // namespace Microsoft::Net::Systemd

#endif // SYSTEMD_RESOLVED_DNSSD_HXX
