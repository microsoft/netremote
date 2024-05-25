
#ifndef IP_ADDRESS_INFORMATION_HXX
#define IP_ADDRESS_INFORMATION_HXX

#include <string>
#include <string_view>

#include <microsoft/net/NetworkInterface.hxx>
#include <microsoft/net/NetworkIpAddress.hxx>

namespace Microsoft::Net
{
/**
 * @brief Information about an IP address needed by the server.
 */
struct IpAddressInformation
{
    NetworkIpFamily Family{ NetworkIpFamily::Unknown };
    NetworkInterfaceType InterfaceType{ NetworkInterfaceType::Unknown };
    std::string InterfaceName;

    auto
    operator<=>(const IpAddressInformation&) const = default;
};
} // namespace Microsoft::Net

namespace std
{
template <>
struct hash<Microsoft::Net::IpAddressInformation>
{
    std::size_t
    operator()(const Microsoft::Net::IpAddressInformation& ipInfo) const noexcept
    {
        return std::hash<int>{}(static_cast<int>(ipInfo.Family)) ^ std::hash<int>{}(static_cast<int>(ipInfo.InterfaceType));
    }
};
} // namespace std

#endif // IP_ADDRESS_INFORMATION_HXX
