
#ifndef SYSTEMD_RESOLVED_DNSSD_HXX
#define SYSTEMD_RESOLVED_DNSSD_HXX

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <microsoft/net/IpAddressInformation.hxx>

namespace Microsoft::Net::Systemd
{
struct ResolvedDnssd
{
    /**
     * @brief Build the TxtText' record for a DNS-SD service.
     *
     * @param ipAddresses The ip addresses to include in the record.
     * @return std::string
     */
    static std::string
    BuildTxtTextRecord(const std::unordered_map<std::string, Microsoft::Net::IpAddressInformation>& ipAddresses);

    /**
     * @brief Register a DNS-SD service with systemd-resolved.
     * 
     * @param serviceName The name of the service.
     * @param protocol The protocol of the service.
     * @param port The port of the service.
     * @param txtTextRecords The TXT records for the service.
     * @param priority The optional priority of the service.
     * @param weight The optional weight of the service.
     * @return std::string 
     */
    static std::string
    RegisterService(std::string_view serviceName, std::string_view protocol, uint16_t port, std::vector<std::string> txtTextRecords, std::optional<uint16_t> priority, std::optional<uint16_t> weight);
};
} // namespace Microsoft::Net::Systemd

#endif // SYSTEMD_RESOLVED_DNSSD_HXX
