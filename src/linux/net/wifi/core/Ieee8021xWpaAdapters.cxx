
#include <Wpa/ProtocolHostapd.hxx>
#include <microsoft/net/Ieee8021xRadiusAuthentication.hxx>

#include "Ieee8021xWpaAdapters.hxx"

namespace Microsoft::Net::Wifi
{
Wpa::RadiusEndpointConfiguration
Ieee8021xRadiusServerEndpointConfigurationToWpaRadiusEndpointConfiguration(const Ieee8021xRadiusServerEndpointConfiguration& ieee8021xRadiusServerEndpointConfiguration) noexcept
{
    Wpa::RadiusEndpointConfiguration wpaRadiusEndpointConfiguration{
        .Type = Ieee8021xRadiusEndpointTypeToWpaRadiusEndpointType(ieee8021xRadiusServerEndpointConfiguration.Type),
        .Address = ieee8021xRadiusServerEndpointConfiguration.Address,
        .Port = ieee8021xRadiusServerEndpointConfiguration.Port,
        .SharedSecret = {
            std::cbegin(ieee8021xRadiusServerEndpointConfiguration.SharedSecret),
            std::cend(ieee8021xRadiusServerEndpointConfiguration.SharedSecret) },
    };

    return wpaRadiusEndpointConfiguration;
}
} // namespace Microsoft::Net::Wifi
