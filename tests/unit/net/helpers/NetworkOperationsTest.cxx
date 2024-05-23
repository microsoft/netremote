
#include <string>
#include <string_view>
#include <unordered_map>

#include <microsoft/net/IpAddressInformation.hxx>
#include <microsoft/net/test/NetworkOperationsTest.hxx>

using namespace Microsoft::Net;
using namespace Microsoft::Net::Test;

std::unordered_map<std::string, IpAddressInformation>
NetworkOperationsTest::GetLocalIpAddressInformation([[maybe_unused]] std::string_view ipAddress) const noexcept
{
    return {};
}
