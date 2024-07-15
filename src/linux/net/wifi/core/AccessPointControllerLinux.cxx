
#include <algorithm>
#include <format>
#include <iterator>
#include <memory>
#include <numeric>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <Wpa/Hostapd.hxx>
#include <Wpa/IHostapd.hxx>
#include <Wpa/ProtocolHostapd.hxx>
#include <logging/LogUtils.hxx>
#include <magic_enum.hpp>
#include <microsoft/net/Ieee8021xRadiusAuthentication.hxx>
#include <microsoft/net/netlink/nl80211/Ieee80211Nl80211Adapters.hxx>
#include <microsoft/net/netlink/nl80211/Netlink80211Wiphy.hxx>
#include <microsoft/net/wifi/AccessPointController.hxx>
#include <microsoft/net/wifi/AccessPointControllerLinux.hxx>
#include <microsoft/net/wifi/AccessPointOperationStatus.hxx>
#include <microsoft/net/wifi/AccessPointOperationStatusLogOnExit.hxx>
#include <microsoft/net/wifi/IAccessPointController.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <microsoft/net/wifi/Ieee80211AccessPointCapabilities.hxx>
#include <plog/Log.h>
#include <plog/Severity.h>

#include "Ieee80211WpaAdapters.hxx"
#include "Ieee8021xWpaAdapters.hxx"

using namespace Microsoft::Net;
using namespace Microsoft::Net::Wifi;

using Microsoft::Net::Netlink::Nl80211::Nl80211Wiphy;
using Wpa::EnforceConfigurationChange;
using Wpa::Hostapd;
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

    // If there is no active hostapd daemon, the operational state is disabled, so status is not needed.
    if (!m_hostapd.IsActive()) {
        operationalState = AccessPointOperationalState::Disabled;
        status.Code = AccessPointOperationStatusCode::Succeeded;
        return status;
    }

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
        AUDITD << std::format("Attempting to set operational state of AP {} to 'enabled'", status.AccessPointId);
        try {
            m_hostapd.Enable();
            status.Code = AccessPointOperationStatusCode::Succeeded;
        } catch (const Wpa::HostapdException& ex) {
            status.Code = AccessPointOperationStatusCode::InternalError;
            status.Details = std::format("failed to set operational state to 'enabled' ({})", ex.what());
        }

        // Validate that enabling the operational state succeeds.
        AccessPointOperationalState actualOperationalState{};
        auto getOperationalStateStatus = GetOperationalState(actualOperationalState);
        if (getOperationalStateStatus.Code == AccessPointOperationStatusCode::Succeeded && actualOperationalState == AccessPointOperationalState::Enabled) {
            AUDITI << std::format("Operational state of AP {} set to 'enabled'", status.AccessPointId);
        }

        break;
    }
    case AccessPointOperationalState::Disabled: {
        AUDITD << std::format("Attempting to set operational state of AP {} to 'disabled'", status.AccessPointId);
        try {
            m_hostapd.Disable();
            status.Code = AccessPointOperationStatusCode::Succeeded;
        } catch (const Wpa::HostapdException& ex) {
            status.Code = AccessPointOperationStatusCode::InternalError;
            status.Details = std::format("failed to set operational state to 'disabled' ({})", ex.what());
        }

        // Validate that disabling the operational state succeeds.
        AccessPointOperationalState actualOperationalState{};
        auto getOperationalStateStatus = GetOperationalState(actualOperationalState);
        if (getOperationalStateStatus.Code == AccessPointOperationStatusCode::Succeeded && actualOperationalState == AccessPointOperationalState::Disabled) {
            AUDITI << std::format("Operational state of AP {} set to 'disabled'", status.AccessPointId);
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

    AUDITD << std::format("Attempting to set PHY type of AP {} to {}", status.AccessPointId, ieeePhyTypeName);

    // Populate a list of required properties to set.
    std::vector<std::pair<std::string_view, std::string_view>> propertiesToSet{};

    // Add the hw_mode property.
    const auto hwMode = IeeePhyTypeToHostapdHwMode(ieeePhyType);
    const auto hwModeValue = HostapdHwModePropertyValue(hwMode);
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

    // There is no way to validate the PHY type that was set, except for AX, AC, and N, which are available from the 'STATUS' command.
    auto hostapdStatus = m_hostapd.GetStatus();
    switch (ieeePhyType) {
    // PHY type AX requires AX, AC, and N to be enabled.
    case Ieee80211PhyType::AX:
        if (hostapdStatus.Ieee80211ax == 1 && hostapdStatus.Disable11ax == 0 &&
            hostapdStatus.Ieee80211ac == 1 && hostapdStatus.Disable11ac == 0 &&
            hostapdStatus.Ieee80211n == 1 && hostapdStatus.Disable11n == 0) {
            AUDITI << std::format("PHY type of AP {} set to 'AX' (HE)", status.AccessPointId);
        }
        break;
    // PHY type AC requires AC and N to be enabled.
    case Ieee80211PhyType::AC:
        if (hostapdStatus.Ieee80211ac == 1 && hostapdStatus.Disable11ac == 0 &&
            hostapdStatus.Ieee80211n == 1 && hostapdStatus.Disable11n == 0) {
            AUDITI << std::format("PHY type of AP {} set to 'AC' (VHT)", status.AccessPointId);
        }
        break;
    // PHY type N only requires N to be enabled.
    case Ieee80211PhyType::N:
        if (hostapdStatus.Ieee80211n == 1 && hostapdStatus.Disable11n == 0) {
            AUDITI << std::format("PHY type of AP {} set to 'N' (HT)", status.AccessPointId);
        }
        break;
    default:
        break;
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

    std::string bands = std::accumulate(std::next(std::begin(frequencyBands)), std::end(frequencyBands), std::string(magic_enum::enum_name(frequencyBands[0])), [](std::string concatenatedBands, Ieee80211FrequencyBand band) {
        return concatenatedBands + ',' + std::string(magic_enum::enum_name(band));
    });
    AUDITD << std::format("Attempting to set frequency bands of AP {} to {}", status.AccessPointId, bands);

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
        m_hostapd.SetProperty(propertyKeyToSet, propertyValueToSet, EnforceConfigurationChange::Defer);
    } catch (const Wpa::HostapdException& ex) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Details = std::format("failed to set hostapd property '{}' to '{}' - {}", propertyKeyToSet, propertyValueToSet, ex.what());
        return status;
    }

    // If 6GHz is included, enable protected management frames, which are required.
    if (std::ranges::contains(frequencyBands, Ieee80211FrequencyBand::SixGHz)) {
        propertyKeyToSet = Wpa::ProtocolHostapd::PropertyNameIeee80211W;
        propertyValueToSet = Wpa::ManagementFrameProtectionToPropertyValue(Wpa::ManagementFrameProtection::Required);

        try {
            m_hostapd.SetProperty(propertyKeyToSet, propertyValueToSet, EnforceConfigurationChange::Defer);
        } catch (const Wpa::HostapdException& ex) {
            status.Code = AccessPointOperationStatusCode::InternalError;
            status.Details = std::format("failed to enable protected management frames for 6GHz - {}", ex.what());
            return status;
        }
    }

    // Validate that the AP is operating on a frequency band that was set by hostapd.
    // TODO: Add parsing code for HostapdStatus::Frequency in ProtocolHostapd.*xx
    /*
    auto hostapdStatus = m_hostapd.GetStatus();
    if (std::ranges::contains(frequencyBands, Ieee80211FrequencyBand::TwoPointFourGHz) && hostapdStatus.Frequency >= 2400 && hostapdStatus.Frequency < 5000) {
        AUDITI << std::format("AP {} is operating on frequency band 2.4GHz with frequency {}", status.AccessPointId, hostapdStatus.Frequency);
    } else if (std::ranges::contains(frequencyBands, Ieee80211FrequencyBand::FiveGHz) && hostapdStatus.Frequency >= 5000 && hostapdStatus.Frequency < 6000) {
        AUDITI << std::format("AP {} is operating on frequency band 5GHz with frequency {}", status.AccessPointId, hostapdStatus.Frequency);
    } else if (std::ranges::contains(frequencyBands, Ieee80211FrequencyBand::SixGHz) && hostapdStatus.Frequency >= 6000 && hostapdStatus.Frequency < 7000) {
        AUDITI << std::format("AP {} is operating on frequency band 6GHz with frequency {}", status.AccessPointId, hostapdStatus.Frequency);
    } else {
        AUDITD << std::format("Frequency bands of AP {} were not set. Current frequency: {}", hostapdStatus.Frequency);
    }
    */

    // Band changes do not require reloading configuration, so skip that step and mark the operation as successful.
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

    std::string algorithms = std::accumulate(std::next(std::begin(authenticationAlgorithms)), std::end(authenticationAlgorithms), std::string(Ieee80211AuthenticationAlgorithmToString(authenticationAlgorithms[0])), [](std::string concatenatedAlgorithms, Ieee80211AuthenticationAlgorithm algorithm) {
        return concatenatedAlgorithms + ',' + std::string(Ieee80211AuthenticationAlgorithmToString(algorithm));
    });
    AUDITD << std::format("Attempting to set authentication algorithms of AP {} to {}", status.AccessPointId, algorithms);

    std::vector<Wpa::WpaAuthenticationAlgorithm> authenticationAlgorithmsHostapd(std::size(authenticationAlgorithms));
    std::ranges::transform(authenticationAlgorithms, std::begin(authenticationAlgorithmsHostapd), Ieee80211AuthenticationAlgorithmToWpaAuthenticationAlgorithm);

    try {
        m_hostapd.SetAuthenticationAlgorithms(authenticationAlgorithmsHostapd, EnforceConfigurationChange::Now);
    } catch (Wpa::HostapdException& ex) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Details = std::format("failed to set authentication algorithms - {}", ex.what());
        return status;
    }

    // TODO: Validate that the AP is using one of the authentication algorithms that were set.
    // Note: This is currently not possible to check in hostapd. Update this in the future if hostapd gets updated.

    status.Code = AccessPointOperationStatusCode::Succeeded;

    return status;
}

AccessPointOperationStatus
AccessPointControllerLinux::SetAuthenticationData(Ieee80211AuthenticationData authenticationData) noexcept
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
        const auto& ieee80211AuthenticationDataPsk = authenticationData.Psk.value();
        auto wpaPreSharedKey = Ieee80211RsnaPskToWpaSharedKey(ieee80211AuthenticationDataPsk.Psk);

        try {
            m_hostapd.SetPreSharedKey(wpaPreSharedKey, EnforceConfigurationChange::Now);
        } catch (const HostapdException& ex) {
            status.Code = AccessPointOperationStatusCode::InternalError;
            status.Details = std::format("failed to set PSK authentication data - {}", ex.what());
            return status;
        }
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

    std::string akms = std::accumulate(std::next(std::begin(akmSuites)), std::end(akmSuites), std::string(magic_enum::enum_name(akmSuites[0])), [](std::string concatenatedAkms, Ieee80211AkmSuite akm) {
        return concatenatedAkms + ',' + std::string(magic_enum::enum_name(akm));
    });
    AUDITD << std::format("Attempting to set AKM suites of AP {} to {}", status.AccessPointId, akms);

    std::vector<Wpa::WpaKeyManagement> wpaKeyManagements(std::size(akmSuites));
    std::ranges::transform(akmSuites, std::begin(wpaKeyManagements), Ieee80211AkmSuiteToWpaKeyManagement);

    try {
        m_hostapd.SetKeyManagement(wpaKeyManagements, EnforceConfigurationChange::Now);
    } catch (HostapdException& ex) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Details = std::format("failed to set akm suites - {}", ex.what());
        return status;
    }

    // Validate that the AKM suites are successfully set by hostapd.
    auto hostapdBssConfiguration = m_hostapd.GetConfiguration();
    auto actualKeyManagements = hostapdBssConfiguration.WpaKeyMgmt;
    if (std::equal(std::begin(wpaKeyManagements), std::end(wpaKeyManagements), std::begin(actualKeyManagements))) {
        AUDITI << std::format("AKM suites of AP {} set to {}", status.AccessPointId, akms);
    } else {
        std::string keyMgmtDebugLog = std::format("AKM suites of AP {} were not set. Current key_mgmt values: ", status.AccessPointId);
        for (const auto& actualKeyManagement : actualKeyManagements) {
            keyMgmtDebugLog += std::format("{} ", std::to_underlying(actualKeyManagement));
        }
        AUDITD << keyMgmtDebugLog;
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

    std::string pairwiseCiphers{};
    for (const auto& [securityProtocol, cipherSuites] : pairwiseCipherSuites) {
        pairwiseCiphers += magic_enum::enum_name(securityProtocol);
        pairwiseCiphers += ": ";
        for (const auto& cipherSuite : cipherSuites) {
            pairwiseCiphers += magic_enum::enum_name(cipherSuite);
            pairwiseCiphers += ",";
        }
        pairwiseCiphers.back() = ';';
    }
    pairwiseCiphers.pop_back(); // Remove trailing semicolon
    AUDITD << std::format("Attempting to set pairwise cipher suites of AP {} to {}", status.AccessPointId, pairwiseCiphers);

    auto pairwiseCipherSuitesHostapd = Ieee80211CipherSuitesToWpaCipherSuites(pairwiseCipherSuites);

    try {
        m_hostapd.SetPairwiseCipherSuites(pairwiseCipherSuitesHostapd, EnforceConfigurationChange::Now);
    } catch (const Wpa::HostapdException& e) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Details = std::format("failed to set pairwise cipher suites - {}", e.what());
        return status;
    }

    // Validate that the pairwise cipher suites were successfully set by hostapd.
    auto hostapdBssConfiguration = m_hostapd.GetConfiguration();
    auto wpaSecurityProtocol = hostapdBssConfiguration.Wpa;
    bool isWpaSecurityProtocolSet = std::ranges::contains(pairwiseCipherSuitesHostapd, wpaSecurityProtocol, [](const auto& wpaCipher) {
        return wpaCipher.first;
    });
    if (isWpaSecurityProtocolSet) {
        AUDITI << std::format("WPA security protocol for AP {} set to {}", status.AccessPointId, std::to_underlying(wpaSecurityProtocol));
    } else {
        AUDITD << std::format("WPA security protocol for AP {} was not set. Current wpa value: {}", status.AccessPointId, std::to_underlying(wpaSecurityProtocol));
    }

    // TODO: Update HostapdBssConfiguration to accept a vector of wpa_pairwise_cipher and rsn_pairwise_cipher, as well as the following code.
    auto wpaPairwiseCipher = hostapdBssConfiguration.WpaPairwiseCipher;
    auto rsnPairwiseCipher = hostapdBssConfiguration.RsnPairwiseCipher;

    auto iter = pairwiseCipherSuitesHostapd.find(wpaSecurityProtocol);
    switch (wpaSecurityProtocol) {
    case Wpa::WpaSecurityProtocol::Wpa:
        if (iter != std::end(pairwiseCipherSuitesHostapd) && std::ranges::contains(iter->second, wpaPairwiseCipher)) {
            AUDITI << std::format("WPA pairwise cipher for AP {} set to {}", status.AccessPointId, std::to_underlying(wpaPairwiseCipher));
        } else {
            AUDITD << std::format("WPA pairwise cipher for AP {} was not set. Current wpa_pairwise_cipher value: {}", status.AccessPointId, std::to_underlying(wpaPairwiseCipher));
        }
        break;
    case Wpa::WpaSecurityProtocol::Wpa2: // Also applies to Wpa3 since they have the same value in the enum class.
        if (iter != std::end(pairwiseCipherSuitesHostapd) && std::ranges::contains(iter->second, rsnPairwiseCipher)) {
            AUDITI << std::format("RSN pairwise cipher for AP {} set to {}", status.AccessPointId, std::to_underlying(rsnPairwiseCipher));
        } else {
            AUDITD << std::format("RSN pairwise cipher for AP {} not set. Current rsn_pairwise_cipher value: {}", status.AccessPointId, std::to_underlying(rsnPairwiseCipher));
        }
        break;
    default:
        break;
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

    AUDITD << std::format("Attempting to set SSID for AP {} to {}", status.AccessPointId, ssid);

    // Attempt to set the SSID.
    try {
        m_hostapd.SetProperty(Wpa::ProtocolHostapd::PropertyNameSsid, ssid, EnforceConfigurationChange::Now);
    } catch (Wpa::HostapdException& ex) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Details = std::format("failed to set 'ssid' property to {} - {}", ssid, ex.what());
        return status;
    }

    // Validate that the SSID was successfully set by hostapd.
    auto hostapdBssConfiguration = m_hostapd.GetConfiguration();
    if (hostapdBssConfiguration.Ssid == ssid) {
        AUDITI << std::format("SSID for AP {} set to {}", status.AccessPointId, hostapdBssConfiguration.Ssid);
    } else {
        AUDITD << std::format("SSID for AP {} was not set. Current ssid value: {}", status.AccessPointId, hostapdBssConfiguration.Ssid);
    }

    status.Code = AccessPointOperationStatusCode::Succeeded;
    return status;
}

AccessPointOperationStatus
AccessPointControllerLinux::SetNetworkBridge([[maybe_unused]] std::string_view networkBridgeId) noexcept
{
    AccessPointOperationStatus status{ GetInterfaceName() };
    const AccessPointOperationStatusLogOnExit logStatusOnExit(&status);

    try {
        m_hostapd.SetBridgeInterface(networkBridgeId, EnforceConfigurationChange::Now);
    } catch (const Wpa::HostapdException& ex) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Details = std::format("failed to set bridge interface to {} - {}", networkBridgeId, ex.what());
        return status;
    }

    status.Code = AccessPointOperationStatusCode::Succeeded;
    return status;
}

AccessPointOperationStatus
AccessPointControllerLinux::SetRadiusConfiguration(Ieee8021xRadiusConfiguration radiusConfiguration) noexcept
{
    AccessPointOperationStatus status{ GetInterfaceName() };
    const AccessPointOperationStatusLogOnExit logStatusOnExit(&status);

    // Allocate space for WPA RADIUS endpoint configurations.
    const bool hasPrimaryAccountingServer = radiusConfiguration.AccountingServer.has_value();
    const std::size_t numRadiusServers = 1 + std::size(radiusConfiguration.FallbackServers) + (hasPrimaryAccountingServer ? 1 : 0);
    std::vector<Wpa::RadiusEndpointConfiguration> radiusEndpointConfigurations{ numRadiusServers };

    // First, convert the primary authentication server.
    auto radiusEndpointConfigurationsIter = std::begin(radiusEndpointConfigurations);
    *radiusEndpointConfigurationsIter++ = Ieee8021xRadiusServerEndpointConfigurationToWpaRadiusEndpointConfiguration(radiusConfiguration.AuthenticationServer);

    // Next, if specified, convert the primary accounting server.
    if (hasPrimaryAccountingServer) {
        *radiusEndpointConfigurationsIter++ = Ieee8021xRadiusServerEndpointConfigurationToWpaRadiusEndpointConfiguration(radiusConfiguration.AccountingServer.value());
    }

    // Last, convert all secondary authentication and accounting servers.
    std::ranges::transform(radiusConfiguration.FallbackServers, radiusEndpointConfigurationsIter, Ieee8021xRadiusServerEndpointConfigurationToWpaRadiusEndpointConfiguration);

    // Attempt to add the WPA RADIUS endpoint configurations.
    try {
        m_hostapd.AddRadiusEndpoints(radiusEndpointConfigurations, EnforceConfigurationChange::Defer);
    } catch (const Wpa::HostapdException& ex) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Details = std::format("failed to add RADIUS endpoint configurations to hostapd - {}", ex.what());
        return status;
    }

    // Disable use of internal EAP server, requiring external RADIUS server for EAP.
    try {
        m_hostapd.SetProperty(Wpa::ProtocolHostapd::PropertyNameEapServer, Wpa::ProtocolHostapd::PropertyDisabled, EnforceConfigurationChange::Defer);
    } catch (const Wpa::HostapdException& ex) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Details = std::format("failed to disable internal EAP server - {}", ex.what());
        return status;
    }

    // Set the "own_ip_addr" property to the IP address of the access point. This value is used as the network access
    // server IP address in RADIUS packets.
    try {
        m_hostapd.SetProperty(Wpa::ProtocolHostapd::PropertyNameOwnIpAddr, m_hostapd.GetIpAddress(), EnforceConfigurationChange::Defer);
    } catch (const Wpa::HostapdException& ex) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Details = std::format("failed to set 'own_ip_addr' property - {}", ex.what());
        return status;
    }

    // If any further RADIUS configuration needs to be applied, it should be done here.

    // Now that all RADIUS configuration has been applied, reload the hostapd configuration to pick up the changes.
    try {
        m_hostapd.Reload();
    } catch (const Wpa::HostapdException& ex) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Details = std::format("failed to reload hostapd configuration for RADIUS configuration change - {}", ex.what());
        return status;
    }

    status.Code = AccessPointOperationStatusCode::Succeeded;
    return status;
}

std::unique_ptr<IAccessPointController>
AccessPointControllerLinuxFactory::Create(std::string_view interfaceName)
{
    if (!Hostapd::IsManagingInterface(interfaceName)) {
        return nullptr;
    }

    return std::make_unique<AccessPointControllerLinux>(interfaceName);
}
