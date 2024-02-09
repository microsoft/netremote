
#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include <thread>

#include <Wpa/Hostapd.hxx>
#include <Wpa/IHostapd.hxx>
#include <Wpa/ProtocolHostapd.hxx>
#include <catch2/catch_test_macros.hpp>

#include "detail/WpaDaemonManager.hxx"

namespace Wpa::Test
{
std::string_view
GetPropertyEnablementValue(int valueToSet)
{
    return (valueToSet != 0) ? ProtocolHostapd::PropertyEnabled : ProtocolHostapd::PropertyDisabled;
}
} // namespace Wpa::Test

TEST_CASE("Create a Hostapd instance (root)", "[wpa][hostapd][client][remote]")
{
    using namespace Wpa;

    SECTION("Create doesn't cause a crash")
    {
        std::optional<Hostapd> hostapd;
        REQUIRE_NOTHROW(hostapd.emplace(WpaDaemonManager::InterfaceNameDefault));
    }

    SECTION("Create multiple for same interface doesn't cause a crash")
    {
        std::optional<Hostapd> hostapd1;
        REQUIRE_NOTHROW(hostapd1.emplace(WpaDaemonManager::InterfaceNameDefault));

        std::optional<Hostapd> hostapd2;
        REQUIRE_NOTHROW(hostapd2.emplace(WpaDaemonManager::InterfaceNameDefault));
    }

    SECTION("Sending command on invalid interface fails")
    {
        Hostapd hostapd("invalid");
        REQUIRE_THROWS_AS(hostapd.Ping(), HostapdException);
    }

    SECTION("Create reflects correct interface name")
    {
        Hostapd hostapd(WpaDaemonManager::InterfaceNameDefault);
        REQUIRE(hostapd.GetInterface() == WpaDaemonManager::InterfaceNameDefault);
    }
}

TEST_CASE("Send Ping() command (root)", "[wpa][hostapd][client][remote]")
{
    using namespace Wpa;

    Hostapd hostapd(WpaDaemonManager::InterfaceNameDefault);

    SECTION("Send Ping() command")
    {
        REQUIRE(hostapd.Ping());
    }

    SECTION("Send Ping() command on second instance")
    {
        Hostapd hostapd2(WpaDaemonManager::InterfaceNameDefault);
        REQUIRE(hostapd2.Ping());
    }
}

TEST_CASE("Send command: GetStatus() (root)", "[wpa][hostapd][client][remote]")
{
    using namespace Wpa;

    Hostapd hostapd(WpaDaemonManager::InterfaceNameDefault);
    REQUIRE(hostapd.Enable());

    SECTION("GetStatus() doesn't throw")
    {
        REQUIRE_NOTHROW(hostapd.GetStatus());
    }

    SECTION("GetStatus() reflects initial 'state' to be operational")
    {
        const auto status = hostapd.GetStatus();
        REQUIRE(IsHostapdStateOperational(status.State));
    }

    SECTION("GetStatus() reflects changes in interface state (disabled -> not disabled)")
    {
        REQUIRE(hostapd.Disable());
        const auto statusInitial = hostapd.GetStatus();
        REQUIRE(statusInitial.State == HostapdInterfaceState::Disabled);
        REQUIRE(hostapd.Enable());
        const auto statusDisabled = hostapd.GetStatus();
        REQUIRE(statusDisabled.State == HostapdInterfaceState::Enabled);
    }

    SECTION("GetStatus() reflects changes in interface state (operational -> not operational)")
    {
        const auto statusInitial = hostapd.GetStatus();
        REQUIRE(IsHostapdStateOperational(statusInitial.State));
        REQUIRE(hostapd.Disable());
        const auto statusDisabled = hostapd.GetStatus();
        REQUIRE(!IsHostapdStateOperational(statusDisabled.State));
    }

    SECTION("GetStatus() reflects changes in IEEE 802.11n state")
    {
        using namespace Wpa::Test;

        const auto ieee80211nInitial = hostapd.GetStatus().Ieee80211n;

        auto ieee80211nValueExpected = !!ieee80211nInitial;
        REQUIRE(hostapd.SetProperty(ProtocolHostapd::PropertyNameIeee80211N, GetPropertyEnablementValue(ieee80211nValueExpected)));
        auto ieee80211nValueUpdated = hostapd.GetStatus().Ieee80211n;
        REQUIRE(ieee80211nValueUpdated == ieee80211nValueExpected);

        ieee80211nValueExpected = !!ieee80211nValueUpdated;
        REQUIRE(hostapd.SetProperty(ProtocolHostapd::PropertyNameIeee80211N, GetPropertyEnablementValue(ieee80211nValueExpected)));
        ieee80211nValueUpdated = hostapd.GetStatus().Ieee80211n;
        REQUIRE(ieee80211nValueUpdated == ieee80211nValueExpected);
    }

    SECTION("GetStatus() reflects changes in IEEE 802.11ac state")
    {
        using namespace Wpa::Test;

        const auto ieee80211acInitial = hostapd.GetStatus().Ieee80211ac;

        auto ieee80211acValueExpected = !!ieee80211acInitial;
        REQUIRE(hostapd.SetProperty(ProtocolHostapd::PropertyNameIeee80211AC, GetPropertyEnablementValue(ieee80211acValueExpected)));
        auto ieee80211acValueUpdated = hostapd.GetStatus().Ieee80211ac;
        REQUIRE(ieee80211acValueUpdated == ieee80211acValueExpected);

        ieee80211acValueExpected = !!ieee80211acValueUpdated;
        REQUIRE(hostapd.SetProperty(ProtocolHostapd::PropertyNameIeee80211AC, GetPropertyEnablementValue(ieee80211acValueExpected)));
        ieee80211acValueUpdated = hostapd.GetStatus().Ieee80211ac;
        REQUIRE(ieee80211acValueUpdated == ieee80211acValueExpected);
    }

    SECTION("GetStatus() reflects changes in IEEE 802.11ax state")
    {
        using namespace Wpa::Test;

        const auto ieee80211axInitial = hostapd.GetStatus().Ieee80211ax;

        auto ieee80211axValueExpected = !!ieee80211axInitial;
        REQUIRE(hostapd.SetProperty(ProtocolHostapd::PropertyNameIeee80211AX, GetPropertyEnablementValue(ieee80211axValueExpected)));
        auto ieee80211axValueUpdated = hostapd.GetStatus().Ieee80211ax;
        REQUIRE(ieee80211axValueUpdated == ieee80211axValueExpected);

        ieee80211axValueExpected = !!ieee80211axValueUpdated;
        REQUIRE(hostapd.SetProperty(ProtocolHostapd::PropertyNameIeee80211AX, GetPropertyEnablementValue(ieee80211axValueExpected)));
        ieee80211axValueUpdated = hostapd.GetStatus().Ieee80211ax;
        REQUIRE(ieee80211axValueUpdated == ieee80211axValueExpected);
    }
}

TEST_CASE("Send GetProperty() command (root)", "[wpa][hostapd][client][remote]")
{
    using namespace Wpa;

    Hostapd hostapd(WpaDaemonManager::InterfaceNameDefault);

    SECTION("GetProperty() doesn't throw")
    {
        std::string whateverValue;
        REQUIRE_NOTHROW(hostapd.GetProperty("whatever", whateverValue));
    }

    SECTION("GetProperty() returns false for invalid property")
    {
        std::string whateverValue;
        REQUIRE_FALSE(hostapd.GetProperty("whatever", whateverValue));
    }

    SECTION("GetProperty() returns true for valid property")
    {
        std::string versionValue;
        REQUIRE(hostapd.GetProperty(ProtocolHostapd::PropertyNameVersion, versionValue));
    }

    SECTION("GetProperty() returns true for valid property with non-empty value")
    {
        std::string versionValue;
        REQUIRE(hostapd.GetProperty(ProtocolHostapd::PropertyNameVersion, versionValue));
        REQUIRE_FALSE(versionValue.empty());
    }

    SECTION("GetProperty() returns correct value for valid property")
    {
        std::string versionValue;
        CHECK(hostapd.GetProperty(ProtocolHostapd::PropertyNameVersion, versionValue));
        REQUIRE(versionValue == ProtocolHostapd::PropertyVersionValue);
    }
}

TEST_CASE("Send SetProperty() command (root)", "[wpa][hostapd][client][remote]")
{
    using namespace Wpa;

    Hostapd hostapd(WpaDaemonManager::InterfaceNameDefault);

    SECTION("SetProperty() doesn't throw")
    {
        REQUIRE_NOTHROW(hostapd.SetProperty("whatever", "whatever"));
    }

    SECTION("SetProperty() returns false for invalid property")
    {
        REQUIRE_FALSE(hostapd.SetProperty("whatever", "whatever"));
    }

    SECTION("SetProperty() returns true for valid property")
    {
        REQUIRE(hostapd.SetProperty(ProtocolHostapd::PropertyNameSetBand, ProtocolHostapd::PropertySetBandValueAuto));
    }

    // TODO: validate that the property was actually set. Need to find a property whose value is retrievable.
}

TEST_CASE("Send control commands: Enable(), Disable() (root)", "[wpa][hostapd][client][remote]")
{
    using namespace Wpa;

    Hostapd hostapd(WpaDaemonManager::InterfaceNameDefault);

    SECTION("Enable() doesn't throw")
    {
        REQUIRE_NOTHROW(hostapd.Enable());
    }

    SECTION("Enable() enables the interface")
    {
        CHECK(hostapd.Disable());
        auto hostapdStatus = hostapd.GetStatus();
        REQUIRE(hostapdStatus.State != HostapdInterfaceState::Enabled);
        REQUIRE(hostapd.Enable());
        hostapdStatus = hostapd.GetStatus();
        REQUIRE(hostapdStatus.State == HostapdInterfaceState::Enabled);
    }

    SECTION("Enable() preserves further communication")
    {
        CHECK(hostapd.Disable());
        CHECK(hostapd.Enable());
        REQUIRE(hostapd.Ping());
    }

    SECTION("Disable() doesn't throw")
    {
        REQUIRE_NOTHROW(hostapd.Disable());
    }

    SECTION("Disable() disables the interface")
    {
        CHECK(hostapd.Disable());
        const auto hostapdStatus = hostapd.GetStatus();
        REQUIRE(hostapdStatus.State == HostapdInterfaceState::Disabled);
    }

    SECTION("Disable() doesn't prevent further communication")
    {
        CHECK(hostapd.Disable());
        REQUIRE(hostapd.Ping());
    }
}

// Note: Terminate() tests kill the hostapd process and so need to have
// individual TEST_CASE()'es to ensure the daemon is started up each time.
// Also, keep Terminate() test cases at end-of-file since Catch2 will run
// tests in declaration order by default, which minimizes the possibility
// of a test case being run after the daemon has been terminated.
TEST_CASE("Send command: Terminate() doesn't throw (root)", "[wpa][hostapd][client][remote]")
{
    using namespace Wpa;

    Hostapd hostapd(WpaDaemonManager::InterfaceNameDefault);
    REQUIRE_NOTHROW(hostapd.Terminate());
}

TEST_CASE("Send command: Terminate() ping failure (root)", "[wpa][hostapd][client][remote]")
{
    using namespace Wpa;
    using namespace std::chrono_literals;

    static constexpr auto TerminationWaitTime{ 2s };

    Hostapd hostapd(WpaDaemonManager::InterfaceNameDefault);
    REQUIRE(hostapd.Ping());
    REQUIRE(hostapd.Terminate());

    // The terminate command merely requests hostapd to shut down. The daemon's
    // polling loop must enter its next cycle before the termination process is
    // executed, so wait a little bit for that to happen.
    std::this_thread::sleep_for(TerminationWaitTime);

    REQUIRE_THROWS_AS(hostapd.Ping(), HostapdException);
}
