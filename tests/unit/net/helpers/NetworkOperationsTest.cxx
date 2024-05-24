
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

#include <microsoft/net/IpAddressInformation.hxx>
#include <microsoft/net/NetworkInterface.hxx>
#include <microsoft/net/NetworkIpAddress.hxx>
#include <microsoft/net/test/NetworkOperationsTest.hxx>

using namespace Microsoft::Net;
using namespace Microsoft::Net::Test;

std::unordered_set<NetworkInterfaceId>
NetworkOperationsTest::EnumerateNetworkInterfaces() const noexcept
{
    return {};
}

std::unordered_set<NetworkIpAddress>
NetworkOperationsTest::GetNetworkInterfaceAddresses([[maybe_unused]] std::string_view networkInterfaceName) const noexcept
{
    return {};
}

std::unordered_map<std::string, IpAddressInformation>
NetworkOperationsTest::GetLocalIpAddressInformation([[maybe_unused]] std::string_view ipAddress) const noexcept
{
    return {};
}
