
#ifndef IEEE_8021X_WPA_ADAPTERS_HXX
#define IEEE_8021X_WPA_ADAPTERS_HXX

#include <Wpa/ProtocolHostapd.hxx>
#include <microsoft/net/Ieee8021xRadiusAuthentication.hxx>

namespace Microsoft::Net::Wifi
{
/**
 * @brief Convert a Ieee8021xRadiusServerEndpointType to a Wpa::RadiusEndpointType.
 *
 * @param ieee8021xRadiusServerEndpointType The Ieee8021xRadiusServerEndpointType to convert.
 * @return constexpr Wpa::RadiusEndpointType The corresponding Wpa::RadiusEndpointType.
 */
constexpr Wpa::RadiusEndpointType
Ieee8021xRadiusEndpointTypeToWpaRadiusEndpointType(Ieee8021xRadiusServerEndpointType ieee8021xRadiusServerEndpointType) noexcept
{
    switch (ieee8021xRadiusServerEndpointType) {
    case Ieee8021xRadiusServerEndpointType::Authentication:
        return Wpa::RadiusEndpointType::Authentication;
    case Ieee8021xRadiusServerEndpointType::Accounting:
        return Wpa::RadiusEndpointType::Accounting;
    case Ieee8021xRadiusServerEndpointType::Unknown:
        [[fallthrough]];
    default:
        return Wpa::RadiusEndpointType::Unknown;
    }
}

/**
 * @brief Convert a Ieee8021xRadiusServerEndpointConfiguration to a Wpa::RadiusEndpointConfiguration.
 *
 * @param ieee8021xRadiusServerEndpointConfiguration The Ieee8021xRadiusServerEndpointConfiguration to convert.
 * @return Wpa::RadiusEndpointConfiguration The corresponding Wpa::RadiusEndpointConfiguration.
 */
Wpa::RadiusEndpointConfiguration
Ieee8021xRadiusServerEndpointConfigurationToWpaRadiusEndpointConfiguration(const Ieee8021xRadiusServerEndpointConfiguration& ieee8021xRadiusServerEndpointConfiguration) noexcept;
} // namespace Microsoft::Net::Wifi

#endif // IEEE_8021X_WPA_ADAPTERS_HXX
