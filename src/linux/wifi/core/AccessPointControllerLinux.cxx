
#include <algorithm>
#include <cstdint>
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
#include <linux/nl80211.h>
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

using namespace Microsoft::Net::Wifi;

using Microsoft::Net::Netlink::Nl80211::Nl80211Wiphy;

AccessPointControllerLinux::AccessPointControllerLinux(std::string_view interfaceName) :
    AccessPointController(interfaceName),
    m_hostapd(interfaceName)
{
}

namespace detail
{
Ieee80211CipherSuite
Nl80211CipherSuiteToIeee80211CipherSuite(uint32_t nl80211CipherSuite) noexcept
{
    return static_cast<Ieee80211CipherSuite>(nl80211CipherSuite);
}

Ieee80211AkmSuite
Nl80211AkmSuiteToIeee80211AkmSuite(uint32_t nl80211AkmSuite) noexcept
{
    return static_cast<Ieee80211AkmSuite>(nl80211AkmSuite);
}

Ieee80211FrequencyBand
Nl80211BandToIeee80211FrequencyBand(nl80211_band nl80211Band) noexcept
{
    switch (nl80211Band) {
    case NL80211_BAND_2GHZ:
        return Ieee80211FrequencyBand::TwoPointFourGHz;
    case NL80211_BAND_5GHZ:
        return Ieee80211FrequencyBand::FiveGHz;
    case NL80211_BAND_60GHZ:
        return Ieee80211FrequencyBand::SixGHz;
    default:
        return Ieee80211FrequencyBand::Unknown;
    }
}

std::vector<Ieee80211Protocol>
Nl80211WiphyToIeee80211Protocols(const Nl80211Wiphy& nl80211Wiphy)
{
    // Ieee80211 B & G are always supported.
    std::vector<Ieee80211Protocol> protocols{
        Ieee80211Protocol::B,
        Ieee80211Protocol::G,
    };

    for (const auto& band : std::views::values(nl80211Wiphy.Bands)) {
        if (band.HtCapabilities != 0) {
            protocols.push_back(Ieee80211Protocol::N);
        }
        if (band.VhtCapabilities != 0) {
            protocols.push_back(Ieee80211Protocol::AC);
        }
        // TODO: once Nl80211WiphyBand is updated to support HE (AX) and EHT (BE), add them here.
    }

    // Remove duplicates.
    std::ranges::sort(protocols);
    protocols.erase(std::ranges::begin(std::ranges::unique(protocols)), std::ranges::end(protocols));

    return protocols;
}

Wpa::HostapdHwMode
IeeeProtocolToHostapdHwMode(Ieee80211Protocol ieeeProtocol)
{
    switch (ieeeProtocol) {
    case Ieee80211Protocol::B:
        return Wpa::HostapdHwMode::Ieee80211b;
    case Ieee80211Protocol::G:
        return Wpa::HostapdHwMode::Ieee80211g;
    case Ieee80211Protocol::N:
        return Wpa::HostapdHwMode::Ieee80211a; // TODO: Could be a or g depending on band
    case Ieee80211Protocol::A:
        return Wpa::HostapdHwMode::Ieee80211a;
    case Ieee80211Protocol::AC:
        return Wpa::HostapdHwMode::Ieee80211a;
    case Ieee80211Protocol::AD:
        return Wpa::HostapdHwMode::Ieee80211ad;
    case Ieee80211Protocol::AX:
        return Wpa::HostapdHwMode::Ieee80211a;
    case Ieee80211Protocol::BE:
        return Wpa::HostapdHwMode::Ieee80211a; // TODO: Assuming a, although hostapd docs don't mention it
    default:
        return Wpa::HostapdHwMode::Unknown;
    }
}

std::string
HostapdHwModeToPropertyValue(Wpa::HostapdHwMode hwMode)
{
    switch (hwMode) {
    case Wpa::HostapdHwMode::Ieee80211b:
        return Wpa::ProtocolHostapd::PropertyHwModeValueB;
    case Wpa::HostapdHwMode::Ieee80211g:
        return Wpa::ProtocolHostapd::PropertyHwModeValueG;
    case Wpa::HostapdHwMode::Ieee80211a:
        return Wpa::ProtocolHostapd::PropertyHwModeValueA;
    case Wpa::HostapdHwMode::Ieee80211ad:
        return Wpa::ProtocolHostapd::PropertyHwModeValueAD;
    case Wpa::HostapdHwMode::Ieee80211any:
        return Wpa::ProtocolHostapd::PropertyHwModeValueAny;
    default: // case Wpa::HostapdHwMode::Unknown
        throw AccessPointControllerException(std::format("Invalid hostapd hw_mode value {}", magic_enum::enum_name(hwMode)));
    }
}

std::string_view
IeeeFrequencyBandToHostapdBand(Ieee80211FrequencyBand ieeeFrequencyBand)
{
    switch (ieeeFrequencyBand) {
    case Ieee80211FrequencyBand::TwoPointFourGHz:
        return Wpa::ProtocolHostapd::PropertySetBandValue2G;
    case Ieee80211FrequencyBand::FiveGHz:
        return Wpa::ProtocolHostapd::PropertySetBandValue5G;
    case Ieee80211FrequencyBand::SixGHz:
        return Wpa::ProtocolHostapd::PropertySetBandValue6G;
    default:
        throw AccessPointControllerException(std::format("Invalid ieee80211 frequency band value {}", magic_enum::enum_name(ieeeFrequencyBand)));
    }
}
} // namespace detail

AccessPointOperationStatus
AccessPointControllerLinux::GetCapabilities(Ieee80211AccessPointCapabilities& ieee80211AccessPointCapabilities)
{
    AccessPointOperationStatus status{ GetInterfaceName(), "GetCapabilities" };
    const AccessPointOperationStatusLogOnExit logStatusOnExit(&status);

    const auto wiphy = Nl80211Wiphy::FromInterfaceName(GetInterfaceName());
    if (!wiphy.has_value()) {
        status.Code = AccessPointOperationStatusCode::AccessPointInvalid;
        status.Details = "failed to get wiphy for interface";
        return status;
    }

    Ieee80211AccessPointCapabilities capabilities{};

    // Convert protocols.
    capabilities.Protocols = detail::Nl80211WiphyToIeee80211Protocols(wiphy.value());

    // Convert frequency bands.
    capabilities.FrequencyBands = std::vector<Ieee80211FrequencyBand>(std::size(wiphy->Bands));
    std::ranges::transform(std::views::keys(wiphy->Bands), std::begin(capabilities.FrequencyBands), detail::Nl80211BandToIeee80211FrequencyBand);

    // Convert AKM suites.
    capabilities.AkmSuites = std::vector<Ieee80211AkmSuite>(std::size(wiphy->AkmSuites));
    std::ranges::transform(wiphy->AkmSuites, std::begin(capabilities.AkmSuites), detail::Nl80211AkmSuiteToIeee80211AkmSuite);

    // Convert cipher suites.
    capabilities.CipherSuites = std::vector<Ieee80211CipherSuite>(std::size(wiphy->CipherSuites));
    std::ranges::transform(wiphy->CipherSuites, std::begin(capabilities.CipherSuites), detail::Nl80211CipherSuiteToIeee80211CipherSuite);

    ieee80211AccessPointCapabilities = std::move(capabilities);

    status.Code = AccessPointOperationStatusCode::Succeeded;

    return status;
}

AccessPointOperationStatus
AccessPointControllerLinux::GetOperationalState(AccessPointOperationalState& operationalState)
{
    AccessPointOperationStatus status{ GetInterfaceName(), "GetOperationalState" };
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
AccessPointControllerLinux::SetOperationalState(AccessPointOperationalState operationalState)
{
    AccessPointOperationStatus status{ GetInterfaceName(), "SetOperationalState" };
    const AccessPointOperationStatusLogOnExit logStatusOnExit(&status);

    switch (operationalState) {
    case AccessPointOperationalState::Enabled: {
        try {
            m_hostapd.Enable();
            status.Code = AccessPointOperationStatusCode::Succeeded;
        } catch (const Wpa::HostapdException& ex) {
            status.Code = AccessPointOperationStatusCode::InternalError;
            status.Details = "failed to set operational state to 'enabled'";
        }
        break;
    }
    case AccessPointOperationalState::Disabled: {
        try {
            m_hostapd.Disable();
            status.Code = AccessPointOperationStatusCode::Succeeded;
        } catch (const Wpa::HostapdException& ex) {
            status.Code = AccessPointOperationStatusCode::InternalError;
            status.Details = "failed to set operational state to 'disabled'";
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
AccessPointControllerLinux::SetProtocol(Ieee80211Protocol ieeeProtocol)
{
    const auto ieeeProtocolName = std::format("802.11 {}", magic_enum::enum_name(ieeeProtocol));
    AccessPointOperationStatus status{ GetInterfaceName(), std::format("SetProtocol {}", ieeeProtocolName).c_str() };
    const AccessPointOperationStatusLogOnExit logStatusOnExit(&status);

    // Populate a list of required properties to set.
    std::vector<std::pair<std::string_view, std::string_view>> propertiesToSet{};

    // Add the hw_mode property.
    const auto hwMode = detail::IeeeProtocolToHostapdHwMode(ieeeProtocol);
    const auto hwModeValue = detail::HostapdHwModeToPropertyValue(hwMode);
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
            hostapdOperationSucceeded = m_hostapd.SetProperty(propertyKeyToSet, propertyValueToSet);
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
    hostapdOperationSucceeded = m_hostapd.Reload();
    if (!hostapdOperationSucceeded) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Details = "failed to reload hostapd configuration";
        return status;
    }

    status.Code = AccessPointOperationStatusCode::Succeeded;

    return status;
}

AccessPointOperationStatus
AccessPointControllerLinux::SetFrequencyBands(std::vector<Ieee80211FrequencyBand> frequencyBands)
{
    AccessPointOperationStatus status{ GetInterfaceName(), "SetFrequencyBands" };
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
        setBandArgumentBuilder << detail::IeeeFrequencyBandToHostapdBand(band) << ',';
    }

    const std::string setBandArgumentAll = setBandArgumentBuilder.str();
    const std::string_view setBandArgument(std::data(setBandArgumentAll), std::size(setBandArgumentAll) - 1); // Remove trailing comma

    bool hostapdOperationSucceeded{ false };
    std::optional<std::string> errorDetails{};
    std::string_view propertyKeyToSet{ Wpa::ProtocolHostapd::PropertyNameSetBand };
    std::string_view propertyValueToSet{ setBandArgument };

    // Set the hostapd "setband" property.
    try {
        hostapdOperationSucceeded = m_hostapd.SetProperty(propertyKeyToSet, propertyValueToSet);
    } catch (const Wpa::HostapdException& ex) {
        hostapdOperationSucceeded = false;
        errorDetails = ex.what();
    }

    if (!hostapdOperationSucceeded) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Details = std::format("failed to set hostapd property '{}' to '{}' - {}", propertyKeyToSet, propertyValueToSet, errorDetails.value_or("unspecified error"));
        return status;
    }

    // Reload hostapd configuration to pick up the changes.
    try {
        hostapdOperationSucceeded = m_hostapd.Reload();
    } catch (const Wpa::HostapdException& ex) {
        hostapdOperationSucceeded = false;
        errorDetails = ex.what();
    }

    if (!hostapdOperationSucceeded) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Details = std::format("failed to reload hostapd configuration - {}", errorDetails.value_or("unspecified error"));
        return status;
    }

    status.Code = AccessPointOperationStatusCode::Succeeded;

    return status;
}

AccessPointOperationStatus
AccessPointControllerLinux::SetSssid(std::string_view ssid)
{
    AccessPointOperationStatus status{ GetInterfaceName(), "SetSsid" };
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
        hostapdOperationSucceeded = m_hostapd.SetProperty(Wpa::ProtocolHostapd::PropertyNameSsid, ssid);
    } catch (Wpa::HostapdException& ex) {
        hostapdOperationSucceeded = false;
        errorDetails = ex.what();
    }

    if (!hostapdOperationSucceeded) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Details = std::format("failed to set 'ssid' property to {} - {}", ssid, errorDetails.value_or("unspecified error"));
        return status;
    }

    // Reload the configuration to pick up the changes.
    try {
        hostapdOperationSucceeded = m_hostapd.Reload();
    } catch (Wpa::HostapdException& ex) {
        hostapdOperationSucceeded = false;
        errorDetails = ex.what();
    }

    if (!hostapdOperationSucceeded) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Details = "failed to reload access point configuration";
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
