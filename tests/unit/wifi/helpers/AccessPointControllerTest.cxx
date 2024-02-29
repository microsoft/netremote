
#include <algorithm>
#include <cassert>
#include <format>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string_view>
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
AccessPointControllerTest::SetProtocol(Ieee80211Protocol ieeeProtocol) noexcept
{
    assert(AccessPoint != nullptr);

    if (AccessPoint == nullptr) {
        return AccessPointOperationStatus::InvalidAccessPoint("null AccessPoint");
    }

    AccessPoint->Protocol = ieeeProtocol;
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
AccessPointControllerTest::SetSssid(std::string_view ssid) noexcept
{
    assert(AccessPoint != nullptr);

    if (AccessPoint == nullptr) {
        return AccessPointOperationStatus::InvalidAccessPoint("null AccessPoint");
    }

    AccessPoint->Ssid = ssid;
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
