
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <magic_enum.hpp>
#include <microsoft/net/IpAddressInformation.hxx>
#include <plog/Log.h>
#include <sdbus-c++/sdbus-c++.h>

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
std::vector<std::unordered_map<std::string, std::vector<uint8_t>>>
ResolvedDnssd::BuildTxtDataRecord(const std::unordered_map<std::string, IpAddressInformation>& ipAddresses)
{
    std::vector<std::unordered_map<std::string, std::vector<uint8_t>>> txtTextRecords{};
    // TODO: convert the ipaddresses to uint8_t vector.
    return txtTextRecords;
}

/* static */
std::string
ResolvedDnssd::RegisterService(std::string_view serviceName, std::string_view protocol, uint16_t port, const std::vector<std::unordered_map<std::string, std::vector<uint8_t>>>& txtDataRecord, std::optional<uint16_t> priority, std::optional<uint16_t> weight)
{
    static constexpr auto Timeout = std::chrono::seconds(2);

    try {
        std::string dnssdObjectPath{};
        auto sdbusProxy = sdbus::createProxy(DbusServiceName, DbusServiceObjectPath, sdbus::dont_run_event_loop_thread);
        sdbusProxy->callMethod(MethodNameReigterService)
            .onInterface(DbusServiceInterface)
            .withArguments(std::data(serviceName), "", std::data(protocol), port, priority.value_or(0), weight.value_or(0), txtDataRecord)
            .withTimeout(Timeout)
            .storeResultsTo(dnssdObjectPath);
        return dnssdObjectPath;
    } catch (const sdbus::Error& sdbusError) {
        LOGE << std::format("Failed to register DNS-SD service with systemd-resolved: {}", sdbusError.what());
    }

    return std::string();
}

/* static */
bool
ResolvedDnssd::UnregisterService([[maybe_unused]] std::string_view serviceObjectPath)
{
    static constexpr auto Timeout = std::chrono::seconds(2);

    try {
        auto sdbusProxy = sdbus::createProxy(DbusServiceName, DbusServiceObjectPath, sdbus::dont_run_event_loop_thread);
        sdbusProxy->callMethod(MethodNameUnregisterService)
            .onInterface(DbusServiceInterface)
            .withArguments(std::data(serviceObjectPath))
            .withTimeout(Timeout);
        return true;
    } catch (const sdbus::Error& sdbusError) {
        LOGE << std::format("Failed to unregister DNS-SD service with systemd-resolved: {}", sdbusError.what());
    }

    return false;
}
