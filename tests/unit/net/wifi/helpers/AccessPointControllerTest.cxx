
#include <algorithm>
#include <cassert>
#include <format>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include <magic_enum.hpp>
#include <microsoft/net/wifi/AccessPointOperationStatus.hxx>
#include <microsoft/net/wifi/IAccessPointController.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <microsoft/net/wifi/Ieee80211AccessPointCapabilities.hxx>
#include <microsoft/net/wifi/test/AccessPointControllerTest.hxx>
#include <microsoft/net/wifi/test/AccessPointTest.hxx>

using namespace Microsoft::Net::Wifi;
using namespace Microsoft::Net::Wifi::Test;

AccessPointControllerTest::AccessPointControllerTest(AccessPointTest *accessPoint) :
    AccessPoint(accessPoint)
{}

std::string_view
AccessPointControllerTest::GetInterfaceName() const noexcept
{
    assert(AccessPoint != nullptr);

    return AccessPoint->InterfaceName;
}

AccessPointOperationStatus
AccessPointControllerTest::GetOperationalState(AccessPointOperationalState &operationalState) noexcept
{
    assert(AccessPoint != nullptr);

    if (AccessPoint == nullptr) {
        return AccessPointOperationStatus::InvalidAccessPoint("null AccessPoint");
    }

    operationalState = AccessPoint->OperationalState;
    return AccessPointOperationStatus::MakeSucceeded(AccessPoint->InterfaceName);
}

AccessPointOperationStatus
AccessPointControllerTest::GetCapabilities(Ieee80211AccessPointCapabilities &ieee80211AccessPointCapabilities) noexcept
{
    assert(AccessPoint != nullptr);

    if (AccessPoint == nullptr) {
        return AccessPointOperationStatus::InvalidAccessPoint("null AccessPoint");
    }

    ieee80211AccessPointCapabilities = AccessPoint->Capabilities;
    return AccessPointOperationStatus::MakeSucceeded(AccessPoint->InterfaceName);
}

AccessPointOperationStatus
AccessPointControllerTest::SetOperationalState(AccessPointOperationalState operationalState) noexcept
{
    assert(AccessPoint != nullptr);

    if (AccessPoint == nullptr) {
        return AccessPointOperationStatus::InvalidAccessPoint("null AccessPoint");
    }

    AccessPoint->OperationalState = operationalState;
    return AccessPointOperationStatus::MakeSucceeded(AccessPoint->InterfaceName);
}

AccessPointOperationStatus
AccessPointControllerTest::SetPhyType(Ieee80211PhyType ieeePhyType) noexcept
{
    assert(AccessPoint != nullptr);

    if (AccessPoint == nullptr) {
        return AccessPointOperationStatus::InvalidAccessPoint("null AccessPoint");
    }

    AccessPoint->PhyType = ieeePhyType;
    return AccessPointOperationStatus::MakeSucceeded(AccessPoint->InterfaceName);
}

AccessPointOperationStatus
AccessPointControllerTest::SetFrequencyBands(std::vector<Ieee80211FrequencyBand> frequencyBands) noexcept
{
    assert(AccessPoint != nullptr);

    if (AccessPoint == nullptr) {
        return AccessPointOperationStatus::InvalidAccessPoint("null AccessPoint");
    }

    for (const auto &frequencyBandToSet : frequencyBands) {
        if (std::ranges::find(AccessPoint->Capabilities.FrequencyBands, frequencyBandToSet) == std::cend(AccessPoint->Capabilities.FrequencyBands)) {
            return {
                AccessPoint->InterfaceName,
                {},
                AccessPointOperationStatusCode::InvalidParameter,
                std::format("AccessPointControllerTest::SetFrequencyBands called with unsupported frequency band {}", magic_enum::enum_name(frequencyBandToSet))
            };
        }
    }

    AccessPoint->FrequencyBands = std::move(frequencyBands);
    return AccessPointOperationStatus::MakeSucceeded(AccessPoint->InterfaceName);
}

AccessPointOperationStatus
AccessPointControllerTest::SetAuthenticationAlgorithms(std::vector<Ieee80211AuthenticationAlgorithm> authenticationAlgorithms) noexcept
{
    assert(AccessPoint != nullptr);

    if (AccessPoint == nullptr) {
        return AccessPointOperationStatus::InvalidAccessPoint("null AccessPoint");
    }

    // TODO: does AccessPointCapabilities need to reflect Ieee80211AuthenticationAlgorithms?
    // If so, ensure authenticationAlgorithms is subset of those in AccessPointCapabilities.AuthenticationAlgorithms.

    AccessPoint->AuthenticationAlgorithms = std::move(authenticationAlgorithms);
    return AccessPointOperationStatus::MakeSucceeded(AccessPoint->InterfaceName);
}

AccessPointOperationStatus
AccessPointControllerTest::SetAuthenticationData(Ieee80211AuthenticationData authenticationData) noexcept
{
    assert(AccessPoint != nullptr);

    if (AccessPoint == nullptr) {
        return AccessPointOperationStatus::InvalidAccessPoint("null AccessPoint");
    }

    AccessPoint->AuthenticationData = std::move(authenticationData);
    return AccessPointOperationStatus::MakeSucceeded(AccessPoint->InterfaceName);
}

AccessPointOperationStatus
AccessPointControllerTest::SetAkmSuites(std::vector<Ieee80211AkmSuite> akmSuites) noexcept
{
    assert(AccessPoint != nullptr);

    if (AccessPoint == nullptr) {
        return AccessPointOperationStatus::InvalidAccessPoint("null AccessPoint");
    }

    AccessPoint->AkmSuites = std::move(akmSuites);
    return AccessPointOperationStatus::MakeSucceeded(AccessPoint->InterfaceName);
}

AccessPointOperationStatus
AccessPointControllerTest::SetPairwiseCipherSuites(std::unordered_map<Ieee80211SecurityProtocol, std::vector<Ieee80211CipherSuite>> pairwiseCipherSuites) noexcept
{
    assert(AccessPoint != nullptr);

    if (AccessPoint == nullptr) {
        return AccessPointOperationStatus::InvalidAccessPoint("null AccessPoint");
    }

    AccessPoint->CipherSuites = std::move(pairwiseCipherSuites);
    return AccessPointOperationStatus::MakeSucceeded(AccessPoint->InterfaceName);
}

AccessPointOperationStatus
AccessPointControllerTest::SetSsid(std::string_view ssid) noexcept
{
    assert(AccessPoint != nullptr);

    if (AccessPoint == nullptr) {
        return AccessPointOperationStatus::InvalidAccessPoint("null AccessPoint");
    }

    AccessPoint->Ssid = ssid;
    return AccessPointOperationStatus::MakeSucceeded(AccessPoint->InterfaceName);
}

AccessPointOperationStatus
AccessPointControllerTest::SetNetworkBridge(std::string_view networkBridgeId) noexcept
{
    assert(AccessPoint != nullptr);

    if (AccessPoint == nullptr) {
        return AccessPointOperationStatus::InvalidAccessPoint("null AccessPoint");
    }

    AccessPoint->BridgeInterfaceId = networkBridgeId;
    return AccessPointOperationStatus::MakeSucceeded(AccessPoint->InterfaceName);
}

AccessPointOperationStatus
AccessPointControllerTest::SetRadiusConfiguration(Ieee8021xRadiusConfiguration radiusConfiguration) noexcept
{
    assert(AccessPoint != nullptr);

    if (AccessPoint == nullptr) {
        return AccessPointOperationStatus::InvalidAccessPoint("null AccessPoint");
    }

    AccessPoint->Authentication8021x.Radius = std::move(radiusConfiguration);
    return AccessPointOperationStatus::MakeSucceeded(AccessPoint->InterfaceName);
}

AccessPointControllerFactoryTest::AccessPointControllerFactoryTest(AccessPointTest *accessPoint) :
    AccessPoint(accessPoint)
{}

std::unique_ptr<IAccessPointController>
AccessPointControllerFactoryTest::Create(std::string_view interfaceName)
{
    assert(AccessPoint != nullptr);

    if (AccessPoint != nullptr && interfaceName != AccessPoint->InterfaceName) {
        throw std::runtime_error("AccessPointControllerFactoryTest::Create called with unexpected interface name");
    }

    return std::make_unique<AccessPointControllerTest>(AccessPoint);
}
