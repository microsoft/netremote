
#include <algorithm>
#include <format>
#include <iterator>
#include <memory>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <Wpa/IHostapd.hxx>
#include <Wpa/ProtocolHostapd.hxx>
#include <magic_enum.hpp>
#include <microsoft/net/netlink/nl80211/Ieee80211Nl80211Adapters.hxx>
#include <microsoft/net/netlink/nl80211/Netlink80211Wiphy.hxx>
#include <microsoft/net/wifi/AccessPointController.hxx>
#include <microsoft/net/wifi/AccessPointControllerLinux.hxx>
#include <microsoft/net/wifi/AccessPointOperationStatus.hxx>
#include <microsoft/net/wifi/AccessPointOperationStatusLogOnExit.hxx>
#include <microsoft/net/wifi/IAccessPointController.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <microsoft/net/wifi/Ieee80211AccessPointCapabilities.hxx>
#include <plog/Severity.h>

#include "Ieee80211WpaAdapters.hxx"

using namespace Microsoft::Net::Wifi;

using Microsoft::Net::Netlink::Nl80211::Nl80211Wiphy;
using Wpa::EnforceConfigurationChange;
using Wpa::HostapdException;

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

    // Convert phy types.
    capabilities.PhyTypes = Nl80211WiphyToIeee80211PhyTypes(wiphy.value());

    // Convert frequency bands.
    capabilities.FrequencyBands = std::vector<Ieee80211FrequencyBand>(std::size(wiphy->Bands));
    std::ranges::transform(std::views::keys(wiphy->Bands), std::begin(capabilities.FrequencyBands), Nl80211BandToIeee80211FrequencyBand);

    // Convert AKM suites.
    capabilities.AkmSuites = std::vector<Ieee80211AkmSuite>(std::size(wiphy->AkmSuites));
    std::ranges::transform(wiphy->AkmSuites, std::begin(capabilities.AkmSuites), Nl80211AkmSuiteToIeee80211AkmSuite);

    // Convert cipher suites.
    capabilities.CipherSuites = std::vector<Ieee80211CipherSuite>(std::size(wiphy->CipherSuites));
    std::ranges::transform(wiphy->CipherSuites, std::begin(capabilities.CipherSuites), Nl80211CipherSuiteToIeee80211CipherSuite);

    // Convert security types.
    capabilities.SecurityProtocols = std::vector<Ieee80211SecurityProtocol>(std::size(wiphy->WpaVersions));
    std::ranges::transform(wiphy->WpaVersions, std::begin(capabilities.SecurityProtocols), Nl80211WpaVersionToIeee80211SecurityProtocol);

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
AccessPointControllerLinux::SetPhyType(Ieee80211PhyType ieeePhyType) noexcept
{
    const auto ieeePhyTypeName = std::format("802.11 {}", magic_enum::enum_name(ieeePhyType));
    AccessPointOperationStatus status{ GetInterfaceName(), std::format("SetPhyType {}", ieeePhyTypeName) };
    const AccessPointOperationStatusLogOnExit logStatusOnExit(&status);

    // Populate a list of required properties to set.
    std::vector<std::pair<std::string_view, std::string_view>> propertiesToSet{};

    // Add the hw_mode property.
    const auto hwMode = IeeePhyTypeToHostapdHwMode(ieeePhyType);
    const auto hwModeValue = HostapdHwModeToPropertyValue(hwMode);
    propertiesToSet.emplace_back(Wpa::ProtocolHostapd::PropertyNameHwMode, hwModeValue);

    // Additively set other hostapd properties based on the protocol.
    switch (ieeePhyType) {
    case Ieee80211PhyType::AX:
        propertiesToSet.emplace_back(Wpa::ProtocolHostapd::PropertyNameIeee80211AX, Wpa::ProtocolHostapd::PropertyEnabled);
        propertiesToSet.emplace_back(Wpa::ProtocolHostapd::PropertyNameDisable11AX, Wpa::ProtocolHostapd::PropertyDisabled);
        [[fallthrough]];
    case Ieee80211PhyType::AC:
        propertiesToSet.emplace_back(Wpa::ProtocolHostapd::PropertyNameIeee80211AC, Wpa::ProtocolHostapd::PropertyEnabled);
        propertiesToSet.emplace_back(Wpa::ProtocolHostapd::PropertyNameDisable11AC, Wpa::ProtocolHostapd::PropertyDisabled);
        [[fallthrough]];
    case Ieee80211PhyType::N:
        propertiesToSet.emplace_back(Wpa::ProtocolHostapd::PropertyNameWmmEnabled, Wpa::ProtocolHostapd::PropertyEnabled);
        propertiesToSet.emplace_back(Wpa::ProtocolHostapd::PropertyNameIeee80211N, Wpa::ProtocolHostapd::PropertyEnabled);
        propertiesToSet.emplace_back(Wpa::ProtocolHostapd::PropertyNameDisable11N, Wpa::ProtocolHostapd::PropertyDisabled);
        break;
    default:
        break;
    }

    std::string_view propertyKeyToSet;
    std::string_view propertyValueToSet;

    // Attempt to set all required properties.
    try {
        for (auto& propertyToSet : propertiesToSet) {
            std::tie(propertyKeyToSet, propertyValueToSet) = std::move(propertyToSet);
            m_hostapd.SetProperty(propertyKeyToSet, propertyValueToSet, EnforceConfigurationChange::Defer);
        }
    } catch (const Wpa::HostapdException& ex) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Details = std::format("failed to set hostapd property '{}' to '{}' - {}", propertyKeyToSet, propertyValueToSet, ex.what());
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

    std::string_view propertyKeyToSet{ Wpa::ProtocolHostapd::PropertyNameSetBand };
    std::string_view propertyValueToSet{ setBandArgument };

    // Set the hostapd "setband" property.
    try {
        m_hostapd.SetProperty(propertyKeyToSet, propertyValueToSet, EnforceConfigurationChange::Now);
    } catch (const Wpa::HostapdException& ex) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Details = std::format("failed to set hostapd property '{}' to '{}' - {}", propertyKeyToSet, propertyValueToSet, ex.what());
        return status;
    }

    status.Code = AccessPointOperationStatusCode::Succeeded;

    return status;
}

AccessPointOperationStatus
AccessPointControllerLinux::SetAuthenticationAlgorithms(std::vector<Ieee80211AuthenticationAlgorithm> authenticationAlgorithms) noexcept
{
    AccessPointOperationStatus status{ GetInterfaceName() };
    const AccessPointOperationStatusLogOnExit logStatusOnExit(&status);

    // Ensure at least one authentication algorithm is requested.
    if (std::empty(authenticationAlgorithms)) {
        status.Code = AccessPointOperationStatusCode::InvalidParameter;
        status.Details = "no authentication algorithms specified";
        return status;
    }

    std::vector<Wpa::WpaAuthenticationAlgorithm> authenticationAlgorithmsHostapd(std::size(authenticationAlgorithms));
    std::ranges::transform(authenticationAlgorithms, std::begin(authenticationAlgorithmsHostapd), Ieee80211AuthenticationAlgorithmToWpaAuthenticationAlgorithm);

    try {
        m_hostapd.SetAuthenticationAlgorithms(authenticationAlgorithmsHostapd, EnforceConfigurationChange::Now);
    } catch (Wpa::HostapdException& ex) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Details = std::format("failed to set authentication algorithms - {}", ex.what());
        return status;
    }

    status.Code = AccessPointOperationStatusCode::Succeeded;

    return status;
}

AccessPointOperationStatus
AccessPointControllerLinux::SetAuthenticationData([[maybe_unused]] Ieee80211AuthenticationData authenticationData) noexcept
{
    AccessPointOperationStatus status{ GetInterfaceName() };
    const AccessPointOperationStatusLogOnExit logStatusOnExit(&status);

    // Ensure at least one set of authentication data is requested.
    if (!authenticationData.Psk.has_value() && !authenticationData.Sae.has_value()) {
        status.Code = AccessPointOperationStatusCode::InvalidParameter;
        status.Details = "no authentication data specified";
        return status;
    }

    if (authenticationData.Psk.has_value()) {
        status.Code = AccessPointOperationStatusCode::OperationNotSupported;
        status.Details = "PSK authentication data is not yet implemented";
        return status;
    }

    if (authenticationData.Sae.has_value()) {
        const auto& ieee80211AuthenticationDataSae = authenticationData.Sae.value();
        std::vector<Wpa::SaePassword> wpaSaePasswords(std::size(ieee80211AuthenticationDataSae.Passwords));
        std::ranges::transform(ieee80211AuthenticationDataSae.Passwords, std::begin(wpaSaePasswords), Ieee80211RsnaPasswordToWpaSaePassword);

        try {
            m_hostapd.SetSaePasswords(std::move(wpaSaePasswords), EnforceConfigurationChange::Now);
        } catch (const HostapdException& ex) {
            status.Code = AccessPointOperationStatusCode::InternalError;
            status.Details = std::format("failed to set SAE passwords - {}", ex.what());
            return status;
        }
    }

    status.Code = AccessPointOperationStatusCode::Succeeded;

    return status;
}

AccessPointOperationStatus
AccessPointControllerLinux::SetAkmSuites(std::vector<Ieee80211AkmSuite> akmSuites) noexcept
{
    AccessPointOperationStatus status{ GetInterfaceName() };
    const AccessPointOperationStatusLogOnExit logStatusOnExit(&status);

    // Ensure at least one akm suite is requested.
    if (std::empty(akmSuites)) {
        status.Code = AccessPointOperationStatusCode::InvalidParameter;
        status.Details = "no akm suites specified";
        return status;
    }

    std::vector<Wpa::WpaKeyManagement> wpaKeyManagements(std::size(akmSuites));
    std::ranges::transform(akmSuites, std::begin(wpaKeyManagements), Ieee80211AkmSuiteToWpaKeyManagement);

    try {
        m_hostapd.SetKeyManagement(wpaKeyManagements, EnforceConfigurationChange::Now);
    } catch (HostapdException& ex) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Details = std::format("failed to set akm suites - {}", ex.what());
        return status;
    }

    status.Code = AccessPointOperationStatusCode::Succeeded;

    return status;
}

AccessPointOperationStatus
AccessPointControllerLinux::SetPairwiseCipherSuites(std::unordered_map<Ieee80211SecurityProtocol, std::vector<Ieee80211CipherSuite>> pairwiseCipherSuites) noexcept
{
    AccessPointOperationStatus status{ GetInterfaceName() };
    const AccessPointOperationStatusLogOnExit logStatusOnExit(&status);

    // Ensure at least one pairwise cipher suite is requested.
    if (std::empty(pairwiseCipherSuites)) {
        status.Code = AccessPointOperationStatusCode::InvalidParameter;
        status.Details = "no pairwise cipher suites specified";
        return status;
    }

    auto pairwiseCipherSuitesHostapd = Ieee80211CipherSuitesToWpaCipherSuites(pairwiseCipherSuites);

    try {
        m_hostapd.SetPairwiseCipherSuites(pairwiseCipherSuitesHostapd, EnforceConfigurationChange::Now);
    } catch (const Wpa::HostapdException& e) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Details = std::format("failed to set pairwise cipher suites - {}", e.what());
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

    // Attempt to set the SSID.
    try {
        m_hostapd.SetProperty(Wpa::ProtocolHostapd::PropertyNameSsid, ssid, EnforceConfigurationChange::Now);
    } catch (Wpa::HostapdException& ex) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Details = std::format("failed to set 'ssid' property to {} - {}", ssid, ex.what());
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
