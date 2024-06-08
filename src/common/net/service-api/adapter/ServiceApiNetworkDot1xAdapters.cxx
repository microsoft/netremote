
#include <iterator>
#include <ranges>

#include <microsoft/net/ServiceApiNetworkDot1xAdapters.hxx>

namespace Microsoft::Net
{
Dot1xRadiusServerEndpointConfiguration
ToServiceDot1xRadiusServerEndpointConfiguration(const Ieee8021xRadiusServerEndpointConfiguration& ieee8021xRadiusServerEndpointConfiguration) noexcept
{
    Dot1xRadiusServerEndpointConfiguration dot1xRadiusServerEndpointConfiguration;
    dot1xRadiusServerEndpointConfiguration.set_endpoint(ToServiceDot1xRadiusServerEndpoint(ieee8021xRadiusServerEndpointConfiguration.Type));
    dot1xRadiusServerEndpointConfiguration.set_address(ieee8021xRadiusServerEndpointConfiguration.Address);
    if (ieee8021xRadiusServerEndpointConfiguration.Port.has_value()) {
        dot1xRadiusServerEndpointConfiguration.set_port(ieee8021xRadiusServerEndpointConfiguration.Port.value());
    }
    *dot1xRadiusServerEndpointConfiguration.mutable_sharedsecret() = {
        std::cbegin(ieee8021xRadiusServerEndpointConfiguration.SharedSecret),
        std::cend(ieee8021xRadiusServerEndpointConfiguration.SharedSecret),
    };

    return dot1xRadiusServerEndpointConfiguration;
}

Ieee8021xRadiusServerEndpointConfiguration
FromServiceDot1xRadiusServerEndpointConfiguration(const Dot1xRadiusServerEndpointConfiguration& dot1xRadiusServerEndpointConfiguration) noexcept
{
    Ieee8021xRadiusServerEndpointConfiguration ieee8021xRadiusServerEndpointConfiguration;
    ieee8021xRadiusServerEndpointConfiguration.Type = FromServiceDot1xRadiusServerEndpoint(dot1xRadiusServerEndpointConfiguration.endpoint());
    ieee8021xRadiusServerEndpointConfiguration.Address = dot1xRadiusServerEndpointConfiguration.address();
    if (dot1xRadiusServerEndpointConfiguration.port() != 0) {
        ieee8021xRadiusServerEndpointConfiguration.Port = dot1xRadiusServerEndpointConfiguration.port();
    }
    ieee8021xRadiusServerEndpointConfiguration.SharedSecret = {
        std::cbegin(dot1xRadiusServerEndpointConfiguration.sharedsecret()),
        std::cend(dot1xRadiusServerEndpointConfiguration.sharedsecret()),
    };

    return ieee8021xRadiusServerEndpointConfiguration;
}

Dot1xRadiusConfiguration
ToServiceDot1xRadiusConfiguration(const Ieee8021xRadiusConfiguration& ieee8021xRadiusConfiguration) noexcept
{
    Dot1xRadiusConfiguration dot1xRadiusConfiguration;
    *dot1xRadiusConfiguration.mutable_authenticationserver() = ToServiceDot1xRadiusServerEndpointConfiguration(ieee8021xRadiusConfiguration.AuthenticationServer);
    if (ieee8021xRadiusConfiguration.AccountingServer.has_value()) {
        *dot1xRadiusConfiguration.mutable_accountingserver() = ToServiceDot1xRadiusServerEndpointConfiguration(ieee8021xRadiusConfiguration.AccountingServer.value());
    }
    if (!std::empty(ieee8021xRadiusConfiguration.FallbackServers)) {
        std::vector<Dot1xRadiusServerEndpointConfiguration> fallbackServers{ std::size(ieee8021xRadiusConfiguration.FallbackServers) };
        std::ranges::transform(ieee8021xRadiusConfiguration.FallbackServers, std::begin(fallbackServers), ToServiceDot1xRadiusServerEndpointConfiguration);
        *dot1xRadiusConfiguration.mutable_fallbackservers() = {
            std::make_move_iterator(std::begin(fallbackServers)),
            std::make_move_iterator(std::end(fallbackServers)),
        };
    }

    return dot1xRadiusConfiguration;
}

Ieee8021xRadiusConfiguration
FromServiceDot1xRadiusConfiguration(const Dot1xRadiusConfiguration& dot1xRadiusConfiguration) noexcept
{
    Ieee8021xRadiusConfiguration ieee8021xRadiusConfiguration;
    if (dot1xRadiusConfiguration.has_authenticationserver()) {
        ieee8021xRadiusConfiguration.AuthenticationServer = FromServiceDot1xRadiusServerEndpointConfiguration(dot1xRadiusConfiguration.authenticationserver());
    }
    if (dot1xRadiusConfiguration.has_accountingserver()) {
        ieee8021xRadiusConfiguration.AccountingServer = FromServiceDot1xRadiusServerEndpointConfiguration(dot1xRadiusConfiguration.accountingserver());
    }
    if (!std::empty(dot1xRadiusConfiguration.fallbackservers())) {
        std::vector<Ieee8021xRadiusServerEndpointConfiguration> fallbackServers{ static_cast<std::size_t>(std::size(dot1xRadiusConfiguration.fallbackservers())) };
        std::ranges::transform(dot1xRadiusConfiguration.fallbackservers(), std::begin(fallbackServers), FromServiceDot1xRadiusServerEndpointConfiguration);
        ieee8021xRadiusConfiguration.FallbackServers = std::move(fallbackServers);
    }

    return ieee8021xRadiusConfiguration;
}
} // namespace Microsoft::Net
