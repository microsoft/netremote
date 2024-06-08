
#ifndef IEEE_8021X_RADIUS_AUTHENTICATION_HXX
#define IEEE_8021X_RADIUS_AUTHENTICATION_HXX

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace Microsoft::Net
{
/**
 * @brief The default RADIUS server port for authentication.
 */
static constexpr uint16_t Ieee8021xRadiusServerAuthenticationPortDefault = 1812;

/**
 * @brief The default RADIUS server port for accounting.
 */
static constexpr uint16_t Ieee8021xRadiusServerAccountingPortDefault = 1813;
/**
 * @brief Type of RADIUS endpoint.
 */
enum class Ieee8021xRadiusServerEndpointType {
    Unknown,
    Authentication,
    Accounting,
};

/**
 * @brief Get the default port for the specified RADIUS server endpoint type.
 *
 * @param endpointType The type of RADIUS server endpoint.
 * @return constexpr uint16_t The default port for the specified RADIUS server endpoint type.
 */
static constexpr uint16_t
GetIeee8021xRadiusDefaultServerPort(Ieee8021xRadiusServerEndpointType endpointType) noexcept
{
    switch (endpointType) {
    case Ieee8021xRadiusServerEndpointType::Authentication:
        return Ieee8021xRadiusServerAuthenticationPortDefault;
    case Ieee8021xRadiusServerEndpointType::Accounting:
        return Ieee8021xRadiusServerAccountingPortDefault;
    default:
        return 0;
    }
}

/**
 * @brief Configuration for a RADIUS endpoint.
 */
struct Ieee8021xRadiusServerEndpointConfiguration
{
    Ieee8021xRadiusServerEndpointType Type{ Ieee8021xRadiusServerEndpointType::Unknown };
    std::string Address;
    std::optional<uint16_t> Port;
    std::vector<uint8_t> SharedSecret;
};

/**
 * @brief Configuration for RADIUS authentication.
 */
struct Ieee8021xRadiusConfiguration
{
    Ieee8021xRadiusServerEndpointConfiguration AuthenticationServer;
    std::optional<Ieee8021xRadiusServerEndpointConfiguration> AccountingServer;
    std::vector<Ieee8021xRadiusServerEndpointConfiguration> ServerFallbacks;
};
} // namespace Microsoft::Net

#endif // IEEE_8021X_RADIUS_AUTHENTICATION_HXX
