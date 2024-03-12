
#include <algorithm>
#include <format>
#include <iterator>
#include <memory>
#include <optional>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

#include <Wpa/IHostapd.hxx>
#include <Wpa/ProtocolHostapd.hxx>
#include <magic_enum.hpp>
#include <microsoft/net/netlink/nl80211/Netlink80211Wiphy.hxx>
#include <microsoft/net/wifi/AccessPointController.hxx>
#include <microsoft/net/wifi/AccessPointControllerLinux.hxx>
#include <microsoft/net/wifi/AccessPointOperationStatus.hxx>
#include <microsoft/net/wifi/AccessPointOperationStatusLogOnExit.hxx>
#include <microsoft/net/wifi/IAccessPointController.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <microsoft/net/wifi/Ieee80211AccessPointCapabilities.hxx>
#include <plog/Severity.h>

#include "Ieee80211Nl80211Adapters.hxx"

using namespace Microsoft::Net::Wifi;

using Microsoft::Net::Netlink::Nl80211::Nl80211Wiphy;
using Wpa::EnforceConfigurationChange;

AccessPointControllerLinux::AccessPointControllerLinux(std::string_view interfaceName) :
    AccessPointController(interfaceName),
    m_hostapd(interfaceName)
{
}

AccessPointOperationStatus
AccessPointControllerLinux::GetCapabilities(Ieee80211AccessPointCapabilities& ieee80211AccessPointCapabilities) noexcept
{
    AccessPointOperationStatus status{ GetInterfaceName() };
    const AccessPointOperationStatusLogOnExit logStatusOnExit(&status);

    const auto wiphy = Nl80211Wiphy::FromInterfaceName(GetInterfaceName());
    if (!wiphy.has_value()) {
        status.Code = AccessPointOperationStatusCode::AccessPointInvalid;
        status.Details = "failed to get wiphy for interface";
        return status;
    }

    Ieee80211AccessPointCapabilities capabilities{};

    // Convert protocols.
    capabilities.Protocols = Nl80211WiphyToIeee80211Protocols(wiphy.value());

    // Convert frequency bands.
    capabilities.FrequencyBands = std::vector<Ieee80211FrequencyBand>(std::size(wiphy->Bands));
    std::ranges::transform(std::views::keys(wiphy->Bands), std::begin(capabilities.FrequencyBands), Nl80211BandToIeee80211FrequencyBand);

    // Convert AKM suites.
    capabilities.AkmSuites = std::vector<Ieee80211AkmSuite>(std::size(wiphy->AkmSuites));
    std::ranges::transform(wiphy->AkmSuites, std::begin(capabilities.AkmSuites), Nl80211AkmSuiteToIeee80211AkmSuite);

    // Convert cipher suites.
    capabilities.CipherSuites = std::vector<Ieee80211CipherSuite>(std::size(wiphy->CipherSuites));
    std::ranges::transform(wiphy->CipherSuites, std::begin(capabilities.CipherSuites), Nl80211CipherSuiteToIeee80211CipherSuite);

    ieee80211AccessPointCapabilities = std::move(capabilities);

    status.Code = AccessPointOperationStatusCode::Succeeded;

    return status;
}

AccessPointOperationStatus
AccessPointControllerLinux::GetOperationalState(AccessPointOperationalState& operationalState) noexcept
{
    AccessPointOperationStatus status{ GetInterfaceName() };
    const AccessPointOperationStatusLogOnExit logStatusOnExit(&status);

    try {
        auto hostapdStatus = m_hostapd.GetStatus();
        operationalState = (hostapdStatus.State == Wpa::HostapdInterfaceState::Enabled)
            ? AccessPointOperationalState::Enabled
            : AccessPointOperationalState::Disabled;
        status.Code = AccessPointOperationStatusCode::Succeeded;
    } catch (const Wpa::HostapdException& ex) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Details = std::format("failed to get operational state - {}", ex.what());
    }

    return status;
}

AccessPointOperationStatus
AccessPointControllerLinux::SetOperationalState(AccessPointOperationalState operationalState) noexcept
{
    AccessPointOperationStatus status{ GetInterfaceName() };
    const AccessPointOperationStatusLogOnExit logStatusOnExit(&status);

    switch (operationalState) {
    case AccessPointOperationalState::Enabled: {
        try {
            m_hostapd.Enable();
            status.Code = AccessPointOperationStatusCode::Succeeded;
        } catch (const Wpa::HostapdException& ex) {
            status.Code = AccessPointOperationStatusCode::InternalError;
            status.Details = std::format("failed to set operational state to 'enabled' ({})", ex.what());
        }
        break;
    }
    case AccessPointOperationalState::Disabled: {
        try {
            m_hostapd.Disable();
            status.Code = AccessPointOperationStatusCode::Succeeded;
        } catch (const Wpa::HostapdException& ex) {
            status.Code = AccessPointOperationStatusCode::InternalError;
            status.Details = std::format("failed to set operational state to 'disabled' ({})", ex.what());
        }
        break;
    }
    default: {
        status.Code = AccessPointOperationStatusCode::InvalidParameter;
        status.Details = std::format("invalid target operational state value '{}'", magic_enum::enum_name(operationalState));
        break;
    }
    }

    return status;
}

AccessPointOperationStatus
AccessPointControllerLinux::SetProtocol(Ieee80211Protocol ieeeProtocol) noexcept
{
    const auto ieeeProtocolName = std::format("802.11 {}", magic_enum::enum_name(ieeeProtocol));
    AccessPointOperationStatus status{ GetInterfaceName(), std::format("SetProtocol {}", ieeeProtocolName) };
    const AccessPointOperationStatusLogOnExit logStatusOnExit(&status);

    // Populate a list of required properties to set.
    std::vector<std::pair<std::string_view, std::string_view>> propertiesToSet{};

    // Add the hw_mode property.
    const auto hwMode = IeeeProtocolToHostapdHwMode(ieeeProtocol);
    const auto hwModeValue = HostapdHwModeToPropertyValue(hwMode);
    propertiesToSet.emplace_back(Wpa::ProtocolHostapd::PropertyNameHwMode, hwModeValue);

    // Additively set other hostapd properties based on the protocol.
    switch (ieeeProtocol) {
    case Ieee80211Protocol::AX:
        propertiesToSet.emplace_back(Wpa::ProtocolHostapd::PropertyNameIeee80211AX, Wpa::ProtocolHostapd::PropertyEnabled);
        propertiesToSet.emplace_back(Wpa::ProtocolHostapd::PropertyNameDisable11AX, Wpa::ProtocolHostapd::PropertyDisabled);
        [[fallthrough]];
    case Ieee80211Protocol::AC:
        propertiesToSet.emplace_back(Wpa::ProtocolHostapd::PropertyNameIeee80211AC, Wpa::ProtocolHostapd::PropertyEnabled);
        propertiesToSet.emplace_back(Wpa::ProtocolHostapd::PropertyNameDisable11AC, Wpa::ProtocolHostapd::PropertyDisabled);
        [[fallthrough]];
    case Ieee80211Protocol::N:
        propertiesToSet.emplace_back(Wpa::ProtocolHostapd::PropertyNameWmmEnabled, Wpa::ProtocolHostapd::PropertyEnabled);
        propertiesToSet.emplace_back(Wpa::ProtocolHostapd::PropertyNameIeee80211N, Wpa::ProtocolHostapd::PropertyEnabled);
        propertiesToSet.emplace_back(Wpa::ProtocolHostapd::PropertyNameDisable11N, Wpa::ProtocolHostapd::PropertyDisabled);
        break;
    default:
        break;
    }

    std::optional<std::string> errorDetails;
    std::string_view propertyKeyToSet;
    std::string_view propertyValueToSet;
    bool hostapdOperationSucceeded{ false };

    // Attempt to set all required properties.
    try {
        for (auto& propertyToSet : propertiesToSet) {
            std::tie(propertyKeyToSet, propertyValueToSet) = std::move(propertyToSet);
            hostapdOperationSucceeded = m_hostapd.SetProperty(propertyKeyToSet, propertyValueToSet, EnforceConfigurationChange::Defer);
            if (!hostapdOperationSucceeded) {
                break;
            }
        }
    } catch (const Wpa::HostapdException& ex) {
        hostapdOperationSucceeded = false;
        errorDetails = ex.what();
    }

    if (!hostapdOperationSucceeded) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Details = std::format("failed to set hostapd property '{}' to '{}' - {}", propertyKeyToSet, propertyValueToSet, errorDetails.value_or("unspecified error"));
        return status;
    }

    // Reload the hostapd configuration to pick up the changes.
    try {
        m_hostapd.Reload();
    } catch (const Wpa::HostapdException& ex) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Details = std::format("failed to reload hostapd configuration - {}", ex.what());
        return status;
    }

    status.Code = AccessPointOperationStatusCode::Succeeded;

    return status;
}

AccessPointOperationStatus
AccessPointControllerLinux::SetFrequencyBands(std::vector<Ieee80211FrequencyBand> frequencyBands) noexcept
{
    AccessPointOperationStatus status{ GetInterfaceName() };
    AccessPointOperationStatusLogOnExit logStatusOnExit(&status);

    // Ensure at least one band is requested.
    if (std::empty(frequencyBands)) {
        status.Code = AccessPointOperationStatusCode::InvalidParameter;
        status.Details = "no frequency bands specified";
        logStatusOnExit.SeverityOnError = plog::Severity::warning;
        return status;
    }

    // Generate the argument for the hostapd "setband" command, which accepts a comma separated list of bands.
    std::ostringstream setBandArgumentBuilder;
    for (const auto& band : frequencyBands) {
        setBandArgumentBuilder << IeeeFrequencyBandToHostapdBand(band) << ',';
    }

    const std::string setBandArgumentAll = setBandArgumentBuilder.str();
    const std::string_view setBandArgument(std::data(setBandArgumentAll), std::size(setBandArgumentAll) - 1); // Remove trailing comma

    bool hostapdOperationSucceeded{ false };
    std::optional<std::string> errorDetails{};
    std::string_view propertyKeyToSet{ Wpa::ProtocolHostapd::PropertyNameSetBand };
    std::string_view propertyValueToSet{ setBandArgument };

    // Set the hostapd "setband" property.
    try {
        hostapdOperationSucceeded = m_hostapd.SetProperty(propertyKeyToSet, propertyValueToSet, EnforceConfigurationChange::Now);
    } catch (const Wpa::HostapdException& ex) {
        hostapdOperationSucceeded = false;
        errorDetails = ex.what();
    }

    if (!hostapdOperationSucceeded) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Details = std::format("failed to set hostapd property '{}' to '{}' - {}", propertyKeyToSet, propertyValueToSet, errorDetails.value_or("unspecified error"));
        return status;
    }

    status.Code = AccessPointOperationStatusCode::Succeeded;

    return status;
}

AccessPointOperationStatus
AccessPointControllerLinux::SetSsid(std::string_view ssid) noexcept
{
    AccessPointOperationStatus status{ GetInterfaceName() };
    const AccessPointOperationStatusLogOnExit logStatusOnExit(&status);

    // Ensure the ssid is not empty.
    if (std::empty(ssid)) {
        status.Code = AccessPointOperationStatusCode::InvalidParameter;
        status.Details = "empty SSID specified";
        return status;
    }

    bool hostapdOperationSucceeded{ false };
    std::optional<std::string> errorDetails{};

    // Attempt to set the SSID.
    try {
        hostapdOperationSucceeded = m_hostapd.SetProperty(Wpa::ProtocolHostapd::PropertyNameSsid, ssid, EnforceConfigurationChange::Now);
    } catch (Wpa::HostapdException& ex) {
        hostapdOperationSucceeded = false;
        errorDetails = ex.what();
    }

    if (!hostapdOperationSucceeded) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Details = std::format("failed to set 'ssid' property to {} - {}", ssid, errorDetails.value_or("unspecified error"));
        return status;
    }

    status.Code = AccessPointOperationStatusCode::Succeeded;
    return status;
}

std::unique_ptr<IAccessPointController>
AccessPointControllerLinuxFactory::Create(std::string_view interfaceName)
{
    return std::make_unique<AccessPointControllerLinux>(interfaceName);
}
