
#include <stdexcept>

#include <microsoft/net/wifi/test/AccessPointControllerTest.hxx>
#include <microsoft/net/wifi/test/AccessPointTest.hxx>

using namespace Microsoft::Net::Wifi;
using namespace Microsoft::Net::Wifi::Test;

AccessPointControllerTest::AccessPointControllerTest(AccessPointTest *accessPoint) :
    AccessPoint(accessPoint)
{}

std::string_view
AccessPointControllerTest::GetInterfaceName() const
{
    if (AccessPoint == nullptr) {
        throw std::runtime_error("AccessPointControllerTest::GetInterfaceName called with null AccessPoint");
    }

    return AccessPoint->InterfaceName;
}

bool
AccessPointControllerTest::GetIsEnabled()
{
    if (AccessPoint == nullptr) {
        throw std::runtime_error("AccessPointControllerTest::GetIsEnabled called with null AccessPoint");
    }

    return AccessPoint->IsEnabled;
}

Ieee80211AccessPointCapabilities
AccessPointControllerTest::GetCapabilities()
{
    if (AccessPoint == nullptr) {
        throw std::runtime_error("AccessPointControllerTest::GetCapabilities called with null AccessPoint");
    }

    return AccessPoint->Capabilities;
}

bool
AccessPointControllerTest::SetProtocol([[maybe_unused]] Microsoft::Net::Wifi::Ieee80211Protocol ieeeProtocol)
{
    if (AccessPoint == nullptr) {
        throw std::runtime_error("AccessPointControllerTest::SetIeeeProtocol called with null AccessPoint");
    }

    // TODO: Set Ieee80211 protocol
    return true;
}

AccessPointControllerFactoryTest::AccessPointControllerFactoryTest(AccessPointTest *accessPoint) :
    AccessPoint(accessPoint)
{}

std::unique_ptr<IAccessPointController>
AccessPointControllerFactoryTest::Create(std::string_view interfaceName)
{
    if (AccessPoint != nullptr && interfaceName != AccessPoint->InterfaceName) {
        throw std::runtime_error("AccessPointControllerFactoryTest::Create called with unexpected interface name");
    }

    return std::make_unique<AccessPointControllerTest>(AccessPoint);
}
