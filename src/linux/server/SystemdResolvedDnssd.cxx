
#include <algorithm>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>

#include <magic_enum.hpp>
#include <microsoft/net/IpAddressInformation.hxx>

#include "SystemdResolvedDnssd.hxx"

using namespace Microsoft::Net::Systemd;
using Microsoft::Net::IpAddressInformation;

namespace detail
{
using Microsoft::Net::NetworkInterfaceType;

/**
 * @brief Convert a NetworkInterfaceType to a string that is compatible with the netremote discovery protocol.
 *
 * @param interfaceType The interface type to convert.
 * @return std::string_view The string representation of the interface type.
 */
std::string_view
GetInterfaceTypeName(NetworkInterfaceType interfaceType)
{
    return magic_enum::enum_name(interfaceType);
}
} // namespace detail

/* static */
std::string
ResolvedDnssd::BuildTxtTextRecord(const std::unordered_map<std::string, IpAddressInformation>& ipAddresses)
{
    std::ostringstream txtTextRecordBuilder{};

    std::ranges::for_each(ipAddresses, [&txtTextRecordBuilder](const auto& ipAddressEntry) {
        const auto& [ipAddress, ipAddressInformation] = ipAddressEntry;
        std::string_view ipAddressKey{ ipAddress };
        auto prefixIndex = ipAddressKey.find_last_of('/');
        if (prefixIndex != std::string_view::npos) {
            ipAddressKey = ipAddressKey.substr(0, prefixIndex);
        }

        txtTextRecordBuilder << ipAddressKey << " " << detail::GetInterfaceTypeName(ipAddressInformation.InterfaceType) << " ";
    });

    auto txtTextRecord = txtTextRecordBuilder.str();
    return txtTextRecord;
}

/* static */
std::string
ResolvedDnssd::RegisterService(std::string_view serviceName, std::string_view protocol, uint16_t port, std::vector<std::string> txtTextRecords, std::optional<uint16_t> priority, std::optional<uint16_t> weight)
{
    // TODO
    return "";
}
