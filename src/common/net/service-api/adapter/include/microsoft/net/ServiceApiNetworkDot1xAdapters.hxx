
#ifndef SERVICE_API_NETWORK_DOT1X_ADAPTERS_HXX
#define SERVICE_API_NETWORK_DOT1X_ADAPTERS_HXX

#include <microsoft/net/Ieee8021xRadiusAuthentication.hxx>
#include <microsoft/net/remote/protocol/Network8021x.pb.h>

namespace Microsoft::Net
{
/**
 * @brief Convert neutral type radius server endpoint to the corresponding service type.
 *
 * @param ieee8021xRadiusServerEndpointType The neutral type radius server endpoint.
 * @return constexpr Dot1xRadiusServerEndpoint The corresponding service type radius server endpoint.
 */
constexpr Dot1xRadiusServerEndpoint
ToServiceDot1xRadiusServerEndpoint(Ieee8021xRadiusServerEndpointType ieee8021xRadiusServerEndpointType) noexcept
{
    switch (ieee8021xRadiusServerEndpointType) {
    case Ieee8021xRadiusServerEndpointType::Authentication:
        return Dot1xRadiusServerEndpointAuthentication;
    case Ieee8021xRadiusServerEndpointType::Accounting:
        return Dot1xRadiusServerEndpointAccounting;
    case Ieee8021xRadiusServerEndpointType::Unknown:
        [[fallthrough]];
    default:
        return Dot1xRadiusServerEndpointUnknown;
    }
}

/**
 * @brief Convert service type radius server endpoint to the corresponding neutral type.
 *
 * @param dot1xRadiusServerEndpoint The service type radius server endpoint.
 * @return constexpr Ieee8021xRadiusServerEndpointType The corresponding neutral type radius server endpoint.
 */
constexpr Ieee8021xRadiusServerEndpointType
FromServiceDot1xRadiusServerEndpoint(Dot1xRadiusServerEndpoint dot1xRadiusServerEndpoint) noexcept
{
    switch (dot1xRadiusServerEndpoint) {
    case Dot1xRadiusServerEndpointAuthentication:
        return Ieee8021xRadiusServerEndpointType::Authentication;
    case Dot1xRadiusServerEndpointAccounting:
        return Ieee8021xRadiusServerEndpointType::Accounting;
    case Dot1xRadiusServerEndpointUnknown:
        [[fallthrough]];
    default:
        return Ieee8021xRadiusServerEndpointType::Unknown;
    }
}

/**
 * @brief Convert neutral type radius server endpoint configuration to the corresponding service type.
 *
 * @param ieee8021xRadiusServerEndpointConfiguration The neutral type radius server endpoint configuration.
 * @return Dot1xRadiusServerEndpointConfiguration The corresponding service type radius server endpoint configuration.
 */
Dot1xRadiusServerEndpointConfiguration
ToServiceDot1xRadiusServerEndpointConfiguration(const Ieee8021xRadiusServerEndpointConfiguration& ieee8021xRadiusServerEndpointConfiguration) noexcept;

/**
 * @brief Convert service type radius server endpoint configuration to the corresponding neutral type.
 *
 * @param dot1xRadiusServerEndpointConfiguration The service type radius server endpoint configuration.
 * @return Ieee8021xRadiusServerEndpointConfiguration The corresponding neutral type radius server endpoint configuration.
 */
Ieee8021xRadiusServerEndpointConfiguration
FromServiceDot1xRadiusServerEndpointConfiguration(const Dot1xRadiusServerEndpointConfiguration& dot1xRadiusServerEndpointConfiguration) noexcept;

/**
 * @brief Convert neutral type radius configuration to the corresponding service type.
 *
 * @param ieee8021xRadiusConfiguration The neutral type radius configuration.
 * @return Dot1xRadiusConfiguration The corresponding service type radius configuration.
 */
Dot1xRadiusConfiguration
ToServiceDot1xRadiusConfiguration(const Ieee8021xRadiusConfiguration& ieee8021xRadiusConfiguration) noexcept;

/**
 * @brief Convert service type radius configuration to the corresponding neutral type.
 *
 * @param dot1xRadiusConfiguration The service type radius configuration.
 * @return Ieee8021xRadiusConfiguration The corresponding neutral type radius configuration.
 */
Ieee8021xRadiusConfiguration
FromServiceDot1xRadiusConfiguration(const Dot1xRadiusConfiguration& dot1xRadiusConfiguration) noexcept;

} // namespace Microsoft::Net

#endif // SERVICE_API_NETWORK_DOT1X_ADAPTERS_HXX
