
#include <format>
#include <string>

#include <microsoft/net/wifi/Ieee80211.hxx>

namespace Microsoft::Net::Wifi
{
std::string
Ieee80211MacAddressToString(const Ieee80211MacAddress& macAddress)
{
    return std::format("{:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}", macAddress[0], macAddress[1], macAddress[2], macAddress[3], macAddress[4], macAddress[5]);
}
} // namespace Microsoft::Net::Wifi
