
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
#include <microsoft/net/wifi/IAccessPointController.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <microsoft/net/wifi/Ieee80211AccessPointCapabilities.hxx>
#include <plog/Log.h>

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

Ieee80211AccessPointCapabilities
AccessPointControllerLinux::GetCapabilities()
{
    auto wiphy = Nl80211Wiphy::FromInterfaceName(GetInterfaceName());
    if (!wiphy.has_value()) {
        throw AccessPointControllerException(std::format("Failed to get wiphy for interface {}", GetInterfaceName()));
    }

    Ieee80211AccessPointCapabilities capabilities;

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

    return capabilities;
}

AccessPointOperationStatus
AccessPointControllerLinux::GetOperationalState(AccessPointOperationalState& operationalState)
{
    AccessPointOperationStatus status{};

    try {
        auto hostapdStatus = m_hostapd.GetStatus();
        operationalState = (hostapdStatus.State == Wpa::HostapdInterfaceState::Enabled)
            ? AccessPointOperationalState::Enabled
            : AccessPointOperationalState::Disabled;
        status = AccessPointOperationStatus::MakeSucceeded();
    } catch (const Wpa::HostapdException& ex) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Message = std::format("Failed to get operational state for interface {} ({})", GetInterfaceName(), ex.what());
    }

    return status;
}

AccessPointOperationStatus
AccessPointControllerLinux::SetOperationalState(AccessPointOperationalState operationalState)
{
    AccessPointOperationStatus status{};

    switch (operationalState) {
    case AccessPointOperationalState::Enabled: {
        try {
            m_hostapd.Enable();
        } catch (const Wpa::HostapdException& ex) {
            status.Code = AccessPointOperationStatusCode::InternalError;
            status.Message = std::format("Failed to set operational state to 'enabled' for {} (unknown error)", GetInterfaceName());
        }
        break;
    }
    case AccessPointOperationalState::Disabled: {
        try {
            m_hostapd.Disable();
        } catch (const Wpa::HostapdException& ex) {
            status.Code = AccessPointOperationStatusCode::InternalError;
            status.Message = std::format("Failed to set operational state to 'disabled' for {} (unknown error)", GetInterfaceName());
        }
        break;
    }
    default: {
        status.Code = AccessPointOperationStatusCode::InvalidParameter;
        status.Message = std::format("Invalid operational state value '{}' for {}", magic_enum::enum_name(operationalState), GetInterfaceName());
        break;
    }
    }

    return status;
}

AccessPointOperationStatus
AccessPointControllerLinux::SetProtocol(Ieee80211Protocol ieeeProtocol)
{
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

    AccessPointOperationStatus status{};
    std::optional<std::string> errorDetails;
    bool hostapdOperationSucceeded{ false };

    // Attempt to set all required properties.
    try {
        for (const auto& [propertyKeyToSet, propertyValueToSet] : propertiesToSet) {
            hostapdOperationSucceeded = m_hostapd.SetProperty(propertyKeyToSet, propertyValueToSet);
            if (!hostapdOperationSucceeded) {
                errorDetails = std::format("failed to set hostapd property '{}' to '{}'", propertyKeyToSet, propertyValueToSet);
                break;
            }
        }
    } catch (const Wpa::HostapdException& ex) {
        hostapdOperationSucceeded = false;
        errorDetails = ex.what();
    }

    // If the required properties were set, reload the hostapd configuration to pick up the changes.
    if (hostapdOperationSucceeded) {
        hostapdOperationSucceeded = m_hostapd.Reload();
        if (!hostapdOperationSucceeded) {
            errorDetails = "failed to reload hostapd configuration";
        }
    }

    const auto ieeeProtocolName = std::format("802.11 {}", magic_enum::enum_name(ieeeProtocol));

    // Finalize return status.
    if (!hostapdOperationSucceeded) {
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Message = std::format("Setting protocol {} for interface {} failed ({})", ieeeProtocolName, GetInterfaceName(), errorDetails.value_or("unspecified error"));
        LOGE << status.Message;
    } else {
        status.Code = AccessPointOperationStatusCode::Succeeded;
        LOGD << std::format("Set protocol {} for interface {}", ieeeProtocolName, GetInterfaceName());
    }

    return status;
}

bool
AccessPointControllerLinux::SetFrequencyBands(std::vector<Ieee80211FrequencyBand> frequencyBands)
{
    // Ensure at least one band is requested.
    if (std::empty(frequencyBands)) {
        LOGW << std::format("No frequency bands specified for interface {}", GetInterfaceName());
        return false;
    }

    // Generate the argument for the hostapd "setband" command, which accepts a comma separated list of bands.
    std::ostringstream setBandArgumentBuilder;
    for (const auto& band : frequencyBands) {
        setBandArgumentBuilder << detail::IeeeFrequencyBandToHostapdBand(band) << ',';
    }

    std::string setBandArgumentAll = setBandArgumentBuilder.str();
    const std::string_view setBandArgument(std::data(setBandArgumentAll), std::size(setBandArgumentAll) - 1); // Remove trailing comma

    bool isOk = false;
    try {
        // Set the hostapd "setband" property.
        isOk = m_hostapd.SetProperty(Wpa::ProtocolHostapd::PropertyNameSetBand, setBandArgument);
        if (!isOk) {
            LOGE << std::format("Failed to set frequency bands for interface {}", GetInterfaceName());
            return false;
        }

        // Reload hostapd configuration to pick up the changes.
        isOk = m_hostapd.Reload();
        if (!isOk) {
            LOGE << std::format("Failed to reload hostapd configuration for interface {}", GetInterfaceName());
            return false;
        }
    } catch (const Wpa::HostapdException& ex) {
        throw AccessPointControllerException(std::format("Failed to set frequency bands for interface {} ({})", GetInterfaceName(), ex.what()));
    }

    return true;
}

AccessPointOperationStatus
AccessPointControllerLinux::SetSssid(std::string_view ssid)
{
    AccessPointOperationStatus status{};

    // Ensure the ssid is not empty.
    if (std::empty(ssid)) {
        status.Code = AccessPointOperationStatusCode::InvalidParameter;
        status.Message = "SSID cannot be empty";
        LOGE << std::format("No SSID specified for interface {}", GetInterfaceName());
        return status;
    }

    // Attempt to set the SSID.
    try {
        const bool propertyWasSet = m_hostapd.SetProperty(Wpa::ProtocolHostapd::PropertyNameSsid, ssid);
        if (!propertyWasSet) {
            LOGE << std::format("Failed to set SSID '{}' for interface {} (rejected)", ssid, GetInterfaceName());
            status.Code = AccessPointOperationStatusCode::InternalError;
            status.Message = std::format("Access point rejected SSID value '{}'", ssid);
            return status;
        }
    } catch (Wpa::HostapdException& ex) {
        LOGE << std::format("Failed to set SSID '{}' for interface {} ({})", ssid, GetInterfaceName(), ex.what());
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Message = ex.what();
        return status;
    }

    // Reload the configuration to pick up the changes.
    try {
        const bool reloaded = m_hostapd.Reload();
        if (!reloaded) {
            LOGE << std::format("Failed to set SSID '{}' for interface {} (configuration reload failed)", ssid, GetInterfaceName());
            status.Code = AccessPointOperationStatusCode::InternalError;
            status.Message = "Failed to reload access point configuration";
            return status;
        }
    } catch (Wpa::HostapdException& ex) {
        LOGE << std::format("Failed to set SSID '{}' for interface {} ({})", ssid, GetInterfaceName(), ex.what());
        status.Code = AccessPointOperationStatusCode::InternalError;
        status.Message = ex.what();
        return status;
    }

    return AccessPointOperationStatus::MakeSucceeded();
}

std::unique_ptr<IAccessPointController>
AccessPointControllerLinuxFactory::Create(std::string_view interfaceName)
{
    return std::make_unique<AccessPointControllerLinux>(interfaceName);
}
