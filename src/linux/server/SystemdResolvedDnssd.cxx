
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <ranges>
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

/**
 * @brief Helper which invokes BuildTxtDataRecordIpAddressKeyValuePair, unwrapping its arguments from a std::pair.
 *
 * @param ipAddressEntry The pair of ip address and ip address information.
 * @return std::pair<std::string, std::vector<uint8_t>>
 */
std::pair<std::string, std::vector<uint8_t>>
BuildTxtDataRecordIpAddressKeyValuePairWrapped(const std::pair<std::string, IpAddressInformation>& ipAddressEntry)
{
    const auto& [ipAddress, ipAddressInformation] = ipAddressEntry;
    return ResolvedDnssd::BuildTxtDataRecordIpAddressKeyValuePair(ipAddress, ipAddressInformation);
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
std::pair<std::string, std::vector<uint8_t>>
ResolvedDnssd::BuildTxtDataRecordIpAddressKeyValuePair(std::string_view ipAddress, const IpAddressInformation& ipAddressInformation)
{
    const auto prefixIndex = ipAddress.find_last_of('/');
    if (prefixIndex != std::string_view::npos) {
        ipAddress = ipAddress.substr(0, prefixIndex);
    }

    const auto interfaceTypeName = detail::GetInterfaceTypeName(ipAddressInformation.InterfaceType);
    std::vector<uint8_t> txtDataValue(std::cbegin(interfaceTypeName), std::cend(interfaceTypeName));

    return std::make_pair(std::string{ ipAddress }, std::move(txtDataValue));
}

/* static */
std::unordered_map<std::string, std::vector<uint8_t>>
ResolvedDnssd::BuildTxtDataRecord(const std::unordered_map<std::string, IpAddressInformation>& ipAddresses)
{
    std::unordered_map<std::string, std::vector<uint8_t>> txtDataRecord{};
    std::ranges::transform(ipAddresses, std::inserter(txtDataRecord, std::end(txtDataRecord)), detail::BuildTxtDataRecordIpAddressKeyValuePairWrapped);

    return txtDataRecord;
}

/* static */
std::string
ResolvedDnssd::RegisterService(std::string_view serviceName, std::string_view protocol, uint16_t port, const std::vector<std::unordered_map<std::string, std::vector<uint8_t>>>& txtDataRecord, std::optional<uint16_t> priority, std::optional<uint16_t> weight)
{
    static constexpr auto Timeout = std::chrono::seconds(2);

    try {
        auto sdbusProxy = sdbus::createProxy(DbusServiceName, DbusServiceObjectPath, sdbus::dont_run_event_loop_thread);
        sdbus::ObjectPath dnssdObjectPath;
        sdbusProxy->callMethod(MethodNameReigterService)
            .onInterface(DbusServiceInterface)
            .withArguments(std::data(serviceName), std::data(serviceName), std::data(protocol), port, priority.value_or(0), weight.value_or(0), txtDataRecord)
            .withTimeout(Timeout)
            .storeResultsTo(dnssdObjectPath);
        LOGD << std::format("Registered DNS-SD service with systemd-resolved: {}", dnssdObjectPath.c_str());
        return dnssdObjectPath;
    } catch (const sdbus::Error& sdbusError) {
        LOGE << std::format("Failed to register DNS-SD service with systemd-resolved: {}", sdbusError.what());
    }

    return std::string();
}

/* static */
bool
ResolvedDnssd::UnregisterService(std::string_view serviceObjectPath)
{
    static constexpr auto Timeout = std::chrono::seconds(2);

    try {
        const sdbus::ObjectPath dnssdObjectPath{ serviceObjectPath };
        auto sdbusProxy = sdbus::createProxy(DbusServiceName, DbusServiceObjectPath, sdbus::dont_run_event_loop_thread);
        sdbusProxy->callMethod(MethodNameUnregisterService)
            .onInterface(DbusServiceInterface)
            .withArguments(dnssdObjectPath)
            .withTimeout(Timeout);
        return true;
    } catch (const sdbus::Error& sdbusError) {
        LOGE << std::format("Failed to unregister DNS-SD service with systemd-resolved: {}", sdbusError.what());
    }

    return false;
}
