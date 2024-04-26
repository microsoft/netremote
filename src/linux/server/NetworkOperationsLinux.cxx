
#include <string>
#include <unordered_set>

#include <microsoft/net/INetworkOperations.hxx>

#include "NetworkOperationsLinux.hxx"

namespace Microsoft::Net
{
std::unordered_set<std::string>
NetworkOperationsLinux::GetLocalIpAddresses() const noexcept
{
    // TODO
    return {};
}

IpInformation
NetworkOperationsLinux::GetLocalIpInformation([[maybe_unused]] const std::string& ipAddress) const noexcept
{
    // TODO
    return {};
}
} // namespace Microsoft::Net
