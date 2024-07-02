
#include <algorithm>
#include <chrono> // NOLINT
#include <cstdint>
#include <initializer_list>
#include <limits>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <Wpa/Hostapd.hxx>
#include <Wpa/IHostapd.hxx>
#include <Wpa/ProtocolHostapd.hxx>
#include <catch2/catch_test_macros.hpp>
#include <magic_enum.hpp>

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

    SECTION("Create with invalid interface name fails")
    {
        std::optional<Hostapd> hostapd;
        REQUIRE_THROWS_AS(hostapd.emplace("invalid"), HostapdException);
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
        REQUIRE_NOTHROW(hostapd.Ping());
    }

    SECTION("Send Ping() command on second instance")
    {
        Hostapd hostapd2(WpaDaemonManager::InterfaceNameDefault);
        REQUIRE_NOTHROW(hostapd2.Ping());
    }
}

TEST_CASE("Send command: GetStatus() (root)", "[wpa][hostapd][client][remote]")
{
    using namespace Wpa;

    Hostapd hostapd(WpaDaemonManager::InterfaceNameDefault);
    REQUIRE_NOTHROW(hostapd.Enable());

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
        REQUIRE_NOTHROW(hostapd.Disable());
        const auto statusInitial = hostapd.GetStatus();
        REQUIRE(statusInitial.State == HostapdInterfaceState::Disabled);
        REQUIRE_NOTHROW(hostapd.Enable());
        const auto statusDisabled = hostapd.GetStatus();
        REQUIRE(statusDisabled.State == HostapdInterfaceState::Enabled);
    }

    SECTION("GetStatus() reflects changes in interface state (operational -> not operational)")
    {
        const auto statusInitial = hostapd.GetStatus();
        REQUIRE(IsHostapdStateOperational(statusInitial.State));
        REQUIRE_NOTHROW(hostapd.Disable());
        const auto statusDisabled = hostapd.GetStatus();
        REQUIRE(!IsHostapdStateOperational(statusDisabled.State));
    }

    SECTION("GetStatus() reflects changes in IEEE 802.11n state")
    {
        using namespace Wpa::Test;

        const auto ieee80211nInitial = hostapd.GetStatus().Ieee80211n;

        auto ieee80211nValueExpected = static_cast<bool>(ieee80211nInitial);
        REQUIRE_NOTHROW(hostapd.SetProperty(ProtocolHostapd::PropertyNameIeee80211N, GetPropertyEnablementValue(ieee80211nValueExpected)));
        auto ieee80211nValueUpdated = hostapd.GetStatus().Ieee80211n;
        REQUIRE(ieee80211nValueUpdated == ieee80211nValueExpected);

        ieee80211nValueExpected = static_cast<bool>(ieee80211nValueUpdated);
        REQUIRE_NOTHROW(hostapd.SetProperty(ProtocolHostapd::PropertyNameIeee80211N, GetPropertyEnablementValue(ieee80211nValueExpected)));
        ieee80211nValueUpdated = hostapd.GetStatus().Ieee80211n;
        REQUIRE(ieee80211nValueUpdated == ieee80211nValueExpected);
    }

    SECTION("GetStatus() reflects changes in IEEE 802.11ac state")
    {
        using namespace Wpa::Test;

        const auto ieee80211acInitial = hostapd.GetStatus().Ieee80211ac;

        auto ieee80211acValueExpected = static_cast<bool>(ieee80211acInitial);
        REQUIRE_NOTHROW(hostapd.SetProperty(ProtocolHostapd::PropertyNameIeee80211AC, GetPropertyEnablementValue(ieee80211acValueExpected)));
        auto ieee80211acValueUpdated = hostapd.GetStatus().Ieee80211ac;
        REQUIRE(ieee80211acValueUpdated == ieee80211acValueExpected);

        ieee80211acValueExpected = static_cast<bool>(ieee80211acValueUpdated);
        REQUIRE_NOTHROW(hostapd.SetProperty(ProtocolHostapd::PropertyNameIeee80211AC, GetPropertyEnablementValue(ieee80211acValueExpected)));
        ieee80211acValueUpdated = hostapd.GetStatus().Ieee80211ac;
        REQUIRE(ieee80211acValueUpdated == ieee80211acValueExpected);
    }

    SECTION("GetStatus() reflects changes in IEEE 802.11ax state")
    {
        using namespace Wpa::Test;

        const auto ieee80211axInitial = hostapd.GetStatus().Ieee80211ax;

        auto ieee80211axValueExpected = static_cast<bool>(ieee80211axInitial);
        REQUIRE_NOTHROW(hostapd.SetProperty(ProtocolHostapd::PropertyNameIeee80211AX, GetPropertyEnablementValue(ieee80211axValueExpected)));
        auto ieee80211axValueUpdated = hostapd.GetStatus().Ieee80211ax;
        REQUIRE(ieee80211axValueUpdated == ieee80211axValueExpected);

        ieee80211axValueExpected = static_cast<bool>(ieee80211axValueUpdated);
        REQUIRE_NOTHROW(hostapd.SetProperty(ProtocolHostapd::PropertyNameIeee80211AX, GetPropertyEnablementValue(ieee80211axValueExpected)));
        ieee80211axValueUpdated = hostapd.GetStatus().Ieee80211ax;
        REQUIRE(ieee80211axValueUpdated == ieee80211axValueExpected);
    }
}

TEST_CASE("Send GetProperty() command (root)", "[wpa][hostapd][client][remote]")
{
    using namespace Wpa;

    Hostapd hostapd(WpaDaemonManager::InterfaceNameDefault);

    SECTION("doesn't throw for valid property")
    {
        REQUIRE_NOTHROW(hostapd.GetProperty(ProtocolHostapd::PropertyNameVersion));
    }

    SECTION("doesn't throw for valid property with non-empty value")
    {
        std::string versionValue;
        REQUIRE_NOTHROW(versionValue = hostapd.GetProperty(ProtocolHostapd::PropertyNameVersion));
        REQUIRE_FALSE(versionValue.empty());
    }

    SECTION("throws for invalid property")
    {
        REQUIRE_THROWS_AS(hostapd.GetProperty("whatever"), HostapdException);
    }

    SECTION("returns correct value for valid property")
    {
        std::string versionValue;
        CHECK_NOTHROW(versionValue = hostapd.GetProperty(ProtocolHostapd::PropertyNameVersion));
        REQUIRE(versionValue == ProtocolHostapd::PropertyVersionValue);
    }
}

TEST_CASE("Send SetProperty() command (root)", "[wpa][hostapd][client][remote]")
{
    using namespace Wpa;

    static constexpr auto PropertyNameInvalid{ "whatever" };
    static constexpr auto PropertyValueInvalid{ "whatever" };

    Hostapd hostapd(WpaDaemonManager::InterfaceNameDefault);

    SECTION("SetProperty() doesn't throw")
    {
        REQUIRE_NOTHROW(hostapd.SetProperty(ProtocolHostapd::PropertyNameSetBand, ProtocolHostapd::PropertySetBandValueAuto, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.SetProperty(ProtocolHostapd::PropertyNameSetBand, ProtocolHostapd::PropertySetBandValueAuto, EnforceConfigurationChange::Defer));
    }

    SECTION("SetProperty() returns false for invalid property")
    {
        REQUIRE_THROWS_AS(hostapd.SetProperty(PropertyNameInvalid, PropertyValueInvalid, EnforceConfigurationChange::Now), HostapdException);
        REQUIRE_THROWS_AS(hostapd.SetProperty(PropertyNameInvalid, PropertyValueInvalid, EnforceConfigurationChange::Defer), HostapdException);
    }

    SECTION("SetProperty() returns true for valid property")
    {
        REQUIRE_NOTHROW(hostapd.SetProperty(ProtocolHostapd::PropertyNameSetBand, ProtocolHostapd::PropertySetBandValueAuto, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.SetProperty(ProtocolHostapd::PropertyNameSetBand, ProtocolHostapd::PropertySetBandValueAuto, EnforceConfigurationChange::Defer));
    }

    SECTION("SetProperty() allows setting a property to the same value")
    {
        REQUIRE_NOTHROW(hostapd.SetProperty(ProtocolHostapd::PropertyNameSetBand, ProtocolHostapd::PropertySetBandValueAuto, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.SetProperty(ProtocolHostapd::PropertyNameSetBand, ProtocolHostapd::PropertySetBandValueAuto, EnforceConfigurationChange::Now));

        REQUIRE_NOTHROW(hostapd.SetProperty(ProtocolHostapd::PropertyNameSetBand, ProtocolHostapd::PropertySetBandValueAuto, EnforceConfigurationChange::Defer));
        REQUIRE_NOTHROW(hostapd.SetProperty(ProtocolHostapd::PropertyNameSetBand, ProtocolHostapd::PropertySetBandValueAuto, EnforceConfigurationChange::Defer));
    }

    SECTION("SetProperty allows setting a property to a different value")
    {
        REQUIRE_NOTHROW(hostapd.SetProperty(ProtocolHostapd::PropertyNameSetBand, ProtocolHostapd::PropertySetBandValueAuto, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.SetProperty(ProtocolHostapd::PropertyNameSetBand, ProtocolHostapd::PropertySetBandValue2G, EnforceConfigurationChange::Now));

        REQUIRE_NOTHROW(hostapd.SetProperty(ProtocolHostapd::PropertyNameSetBand, ProtocolHostapd::PropertySetBandValue5G, EnforceConfigurationChange::Defer));
        REQUIRE_NOTHROW(hostapd.SetProperty(ProtocolHostapd::PropertyNameSetBand, ProtocolHostapd::PropertySetBandValue6G, EnforceConfigurationChange::Defer));
    }

    SECTION("SetProperty allows interleaving enforcement of configuration changes")
    {
        REQUIRE_NOTHROW(hostapd.SetProperty(ProtocolHostapd::PropertyNameSetBand, ProtocolHostapd::PropertySetBandValueAuto, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.SetProperty(ProtocolHostapd::PropertyNameSetBand, ProtocolHostapd::PropertySetBandValue2G, EnforceConfigurationChange::Defer));
        REQUIRE_NOTHROW(hostapd.SetProperty(ProtocolHostapd::PropertyNameSetBand, ProtocolHostapd::PropertySetBandValue5G, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.SetProperty(ProtocolHostapd::PropertyNameSetBand, ProtocolHostapd::PropertySetBandValue6G, EnforceConfigurationChange::Defer));
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
        CHECK_NOTHROW(hostapd.Disable());
        auto hostapdStatus = hostapd.GetStatus();
        REQUIRE(hostapdStatus.State != HostapdInterfaceState::Enabled);
        REQUIRE_NOTHROW(hostapd.Enable());
        hostapdStatus = hostapd.GetStatus();
        REQUIRE(hostapdStatus.State == HostapdInterfaceState::Enabled);
    }

    SECTION("Enable() preserves further communication")
    {
        CHECK_NOTHROW(hostapd.Disable());
        CHECK_NOTHROW(hostapd.Enable());
        REQUIRE_NOTHROW(hostapd.Ping());
    }

    SECTION("Disable() doesn't throw")
    {
        REQUIRE_NOTHROW(hostapd.Disable());
    }

    SECTION("Disable() disables the interface")
    {
        CHECK_NOTHROW(hostapd.Disable());
        const auto hostapdStatus = hostapd.GetStatus();
        REQUIRE(hostapdStatus.State == HostapdInterfaceState::Disabled);
    }

    SECTION("Disable() doesn't prevent further communication")
    {
        CHECK_NOTHROW(hostapd.Disable());
        REQUIRE_NOTHROW(hostapd.Ping());
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

    static constexpr auto TerminationWaitTime{ 2s }; // NOLINT

    Hostapd hostapd(WpaDaemonManager::InterfaceNameDefault);
    REQUIRE_NOTHROW(hostapd.Ping());
    REQUIRE_NOTHROW(hostapd.Terminate());

    // The terminate command merely requests hostapd to shut down. The daemon's
    // polling loop must enter its next cycle before the termination process is
    // executed, so wait a little bit for that to happen.
    std::this_thread::sleep_for(TerminationWaitTime);

    REQUIRE_THROWS_AS(hostapd.Ping(), HostapdException);
}

TEST_CASE("Send SetSsid() command (root)", "[wpa][hostapd][client][remote]")
{
    using namespace Wpa;

    constexpr auto SsidValid{ "whatever" };
    constexpr auto SsidInvalidTooLong{ "This SSID is way too long to be valid because it has more than 32 characters" };

    Hostapd hostapd(WpaDaemonManager::InterfaceNameDefault);

    SECTION("doesn't throw for valid SSID")
    {
        REQUIRE_NOTHROW(hostapd.SetSsid(SsidValid));
    }

    SECTION("SetSsid() throws for empty SSID")
    {
        REQUIRE_THROWS_AS(hostapd.SetSsid(""), HostapdException);
    }

    SECTION("SetSsid() throws for SSID with length > 32")
    {
        REQUIRE_THROWS_AS(hostapd.SetSsid(SsidInvalidTooLong), HostapdException);
    }

    SECTION("SetSsid() changes the SSID for valid input")
    {
        constexpr auto SsidToSet{ SsidValid };
        REQUIRE_NOTHROW(hostapd.SetSsid(SsidToSet));
        const auto status = hostapd.GetStatus();
        REQUIRE(!std::empty(status.Bss));
        REQUIRE(status.Bss[0].Ssid == SsidToSet);
    }

    // Note: The below tests use starts_with() to check the SSID instead of == as hostapd has a bug where attempting to
    // provide an invalid SSID results in the reported SSID having some junk characters following the real SSID.

    SECTION("SetSsid() does not change the SSID for invalid inputs")
    {
        const auto statusInitial = hostapd.GetStatus();
        REQUIRE(!std::empty(statusInitial.Bss));

        const auto* const SsidToSet{ SsidInvalidTooLong };
        REQUIRE_THROWS_AS(hostapd.SetSsid(SsidToSet), HostapdException);

        const auto statusAfterFail = hostapd.GetStatus();
        REQUIRE(!std::empty(statusAfterFail.Bss));
        REQUIRE(statusAfterFail.Bss[0].Ssid.starts_with(statusInitial.Bss[0].Ssid));
    }
}

TEST_CASE("Send SetWpaSecurityProtocols() command (root)", "[wpa][hostapd][client][remote]")
{
    using namespace Wpa;

    // NOLINTNEXTLINE
    static constexpr auto WpaSecurityProtocolInvalid = static_cast<WpaSecurityProtocol>(std::numeric_limits<std::underlying_type_t<WpaSecurityProtocol>>::max());

    Hostapd hostapd(WpaDaemonManager::InterfaceNameDefault);

    SECTION("Doesn't throw")
    {
        REQUIRE_NOTHROW(hostapd.SetWpaSecurityProtocols({ WpaSecurityProtocol::Wpa }, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.SetWpaSecurityProtocols({ WpaSecurityProtocol::Wpa, WpaSecurityProtocol::Wpa2 }, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.SetWpaSecurityProtocols({ WpaSecurityProtocol::Wpa }, EnforceConfigurationChange::Defer));
        REQUIRE_NOTHROW(hostapd.SetWpaSecurityProtocols({ WpaSecurityProtocol::Wpa, WpaSecurityProtocol::Wpa2 }, EnforceConfigurationChange::Defer));
    }

    SECTION("Fails with empty input")
    {
        REQUIRE_THROWS_AS(hostapd.SetWpaSecurityProtocols({}, EnforceConfigurationChange::Now), HostapdException);
    }

    SECTION("Fails with invalid input")
    {
        REQUIRE_THROWS_AS(hostapd.SetWpaSecurityProtocols({ WpaSecurityProtocolInvalid }, EnforceConfigurationChange::Now), HostapdException);
        REQUIRE_THROWS_AS(hostapd.SetWpaSecurityProtocols({ WpaSecurityProtocol::Wpa, WpaSecurityProtocolInvalid }, EnforceConfigurationChange::Now), HostapdException);
        REQUIRE_THROWS_AS(hostapd.SetWpaSecurityProtocols({ WpaSecurityProtocol::Wpa, WpaSecurityProtocolInvalid, WpaSecurityProtocol::Wpa2 }, EnforceConfigurationChange::Now), HostapdException);

        REQUIRE_THROWS_AS(hostapd.SetWpaSecurityProtocols({ WpaSecurityProtocolInvalid }, EnforceConfigurationChange::Defer), HostapdException);
        REQUIRE_THROWS_AS(hostapd.SetWpaSecurityProtocols({ WpaSecurityProtocol::Wpa, WpaSecurityProtocolInvalid }, EnforceConfigurationChange::Defer), HostapdException);
        REQUIRE_THROWS_AS(hostapd.SetWpaSecurityProtocols({ WpaSecurityProtocol::Wpa, WpaSecurityProtocolInvalid, WpaSecurityProtocol::Wpa2 }, EnforceConfigurationChange::Defer), HostapdException);
    }

    SECTION("Succeeds with valid, single input")
    {
        REQUIRE_NOTHROW(hostapd.SetWpaSecurityProtocols({ WpaSecurityProtocol::Wpa }, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.SetWpaSecurityProtocols({ WpaSecurityProtocol::Wpa }, EnforceConfigurationChange::Defer));
    }

    SECTION("Succeeds with valid, multiple inputs")
    {
        REQUIRE_NOTHROW(hostapd.SetWpaSecurityProtocols({ WpaSecurityProtocol::Wpa, WpaSecurityProtocol::Wpa2 }, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.SetWpaSecurityProtocols({ WpaSecurityProtocol::Wpa, WpaSecurityProtocol::Wpa2 }, EnforceConfigurationChange::Defer));
    }

    SECTION("Succeeds with valid, duplicate inputs")
    {
        REQUIRE_NOTHROW(hostapd.SetWpaSecurityProtocols({ WpaSecurityProtocol::Wpa, WpaSecurityProtocol::Wpa }, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.SetWpaSecurityProtocols({ WpaSecurityProtocol::Wpa, WpaSecurityProtocol::Wpa }, EnforceConfigurationChange::Defer));

        REQUIRE_NOTHROW(hostapd.SetWpaSecurityProtocols({ WpaSecurityProtocol::Wpa2, WpaSecurityProtocol::Wpa2 }, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.SetWpaSecurityProtocols({ WpaSecurityProtocol::Wpa2, WpaSecurityProtocol::Wpa2 }, EnforceConfigurationChange::Defer));
    }
}

TEST_CASE("Send SetKeyManagement() command (root)", "[wpa][hostapd][client][remote]")
{
    using namespace Wpa;

    static constexpr std::initializer_list<WpaKeyManagement> KeyManagementInvalidValues = {
        WpaKeyManagement::None,
        WpaKeyManagement::Ieee8021xNoWpa,
        WpaKeyManagement::WpaNone,
        WpaKeyManagement::Wps,
        WpaKeyManagement::WapiPsk,
        WpaKeyManagement::WapiCert,
        WpaKeyManagement::Cckm,
    };

    static constexpr std::initializer_list<WpaKeyManagement> KeyManagementValidValues = {
        WpaKeyManagement::Ieee8021x,
        WpaKeyManagement::Psk,
        WpaKeyManagement::FtIeee8021x,
        WpaKeyManagement::FtPsk,
        WpaKeyManagement::Ieee8021xSha256,
        WpaKeyManagement::PskSha256,
        WpaKeyManagement::Sae,
        WpaKeyManagement::FtSae,
        WpaKeyManagement::Ieee8021xSuiteB,
        WpaKeyManagement::Ieee8021xSuiteB192,
        WpaKeyManagement::FilsSha256,
        WpaKeyManagement::FilsSha384,
        WpaKeyManagement::FtFilsSha256,
        WpaKeyManagement::FtFilsSha384,
        WpaKeyManagement::Owe,
        WpaKeyManagement::Dpp,
        WpaKeyManagement::FtIeee8021xSha384,
        // WpaKeyManagement::Osen,                  // Only applies to hotspot 2.0, not needed.
        // WpaKeyManagement::Pasn,                  // hostapd v2.10 doesn't compile with this feature enabled (CONFIG_PASN=y)
    };

    Hostapd hostapd(WpaDaemonManager::InterfaceNameDefault);

    // If any of the key management values are IEEE 802.1X, add a RADIUS server, otherwise hostapd will refuse to
    // allow EAP-based AKMs (at least one EAP authenticator must be pre-configured).
    if (std::ranges::any_of(KeyManagementValidValues, IsKeyManagementIeee8021x)) {
        RadiusEndpointConfiguration radiusAuthenticationServerEndpointConfiguration{
            .Type = RadiusEndpointType::Authentication,
            .Address = "127.0.0.1",
            .Port = 1812,
            .SharedSecret = "radius_secret",
        };
        REQUIRE_NOTHROW(hostapd.AddRadiusEndpoints({ std::move(radiusAuthenticationServerEndpointConfiguration) }, EnforceConfigurationChange::Defer));
    }

    SECTION("Doesn't throw")
    {
        REQUIRE_NOTHROW(hostapd.SetKeyManagement({ WpaKeyManagement::Psk }, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.SetKeyManagement({ WpaKeyManagement::Psk }, EnforceConfigurationChange::Defer));
        REQUIRE_NOTHROW(hostapd.SetKeyManagement({ WpaKeyManagement::Psk, WpaKeyManagement::Sae }, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.SetKeyManagement({ WpaKeyManagement::Psk, WpaKeyManagement::Sae }, EnforceConfigurationChange::Defer));
    }

    SECTION("Fails with empty input")
    {
        REQUIRE_THROWS_AS(hostapd.SetKeyManagement({}, EnforceConfigurationChange::Now), HostapdException);
    }

    SECTION("Fails with every invalid input")
    {
        std::vector<WpaKeyManagement> keyManagementInvalidValues{};
        for (const auto keyManagementInvalidValue : KeyManagementInvalidValues) {
            keyManagementInvalidValues.push_back(keyManagementInvalidValue);
            REQUIRE_THROWS_AS(hostapd.SetKeyManagement({ keyManagementInvalidValue }, EnforceConfigurationChange::Now), HostapdException);
            REQUIRE_THROWS_AS(hostapd.SetKeyManagement({ keyManagementInvalidValue }, EnforceConfigurationChange::Defer), HostapdException);
            REQUIRE_THROWS_AS(hostapd.SetKeyManagement(keyManagementInvalidValues, EnforceConfigurationChange::Now), HostapdException);
            REQUIRE_THROWS_AS(hostapd.SetKeyManagement(keyManagementInvalidValues, EnforceConfigurationChange::Defer), HostapdException);
        }
    }

    SECTION("Succeeds with all valid, single inputs")
    {
        for (const auto keyManagementValidValue : KeyManagementValidValues) {
            REQUIRE_NOTHROW(hostapd.SetKeyManagement({ keyManagementValidValue }, EnforceConfigurationChange::Now));
            REQUIRE_NOTHROW(hostapd.SetKeyManagement({ keyManagementValidValue }, EnforceConfigurationChange::Defer));
        }
    }

    SECTION("Succeeds with valid, multiple inputs")
    {
        std::vector<WpaKeyManagement> keyManagementValidValues{};
        for (const auto keyManagementValidValue : KeyManagementValidValues) {
            keyManagementValidValues.push_back(keyManagementValidValue);
            REQUIRE_NOTHROW(hostapd.SetKeyManagement(keyManagementValidValues, EnforceConfigurationChange::Now));
            REQUIRE_NOTHROW(hostapd.SetKeyManagement(keyManagementValidValues, EnforceConfigurationChange::Defer));
        }
    }

    SECTION("Succeeds with valid, duplicate inputs")
    {
        for (const auto keyManagementValidValue : KeyManagementValidValues) {
            REQUIRE_NOTHROW(hostapd.SetKeyManagement({ keyManagementValidValue, keyManagementValidValue }, EnforceConfigurationChange::Now));
            REQUIRE_NOTHROW(hostapd.SetKeyManagement({ keyManagementValidValue, keyManagementValidValue }, EnforceConfigurationChange::Defer));
        }
    }
}

TEST_CASE("Send SetPairwiseCipherSuites() command (root)", "[wpa][hostapd][client][remote]")
{
    using namespace Wpa;

    Hostapd hostapd(WpaDaemonManager::InterfaceNameDefault);

    SECTION("Doesn't throw")
    {
        REQUIRE_NOTHROW(hostapd.SetPairwiseCipherSuites(WpaSecurityProtocol::Wpa, { WpaCipher::Ccmp }, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.SetPairwiseCipherSuites(WpaSecurityProtocol::Wpa, { WpaCipher::Ccmp }, EnforceConfigurationChange::Defer));
        REQUIRE_NOTHROW(hostapd.SetPairwiseCipherSuites({ { WpaSecurityProtocol::Wpa, { WpaCipher::Ccmp } } }, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.SetPairwiseCipherSuites({ { WpaSecurityProtocol::Wpa, { WpaCipher::Ccmp } } }, EnforceConfigurationChange::Defer));
    }

    SECTION("Fails with empty inputs")
    {
        // Empty cipher list.
        REQUIRE_THROWS_AS(hostapd.SetPairwiseCipherSuites(WpaSecurityProtocol::Wpa, {}, EnforceConfigurationChange::Now), HostapdException);
        REQUIRE_THROWS_AS(hostapd.SetPairwiseCipherSuites(WpaSecurityProtocol::Wpa, {}, EnforceConfigurationChange::Defer), HostapdException);

        // Empty protocol list.
        REQUIRE_THROWS_AS(hostapd.SetPairwiseCipherSuites({}, EnforceConfigurationChange::Now), HostapdException);
        REQUIRE_THROWS_AS(hostapd.SetPairwiseCipherSuites({}, EnforceConfigurationChange::Defer), HostapdException);

        // clang-format off
        // Empty protocol list in map entry.
        REQUIRE_THROWS_AS(hostapd.SetPairwiseCipherSuites({
            { WpaSecurityProtocol::Wpa, {} },
            { WpaSecurityProtocol::Wpa2, { WpaCipher::Aes128Cmac } },
        }, EnforceConfigurationChange::Now), HostapdException);
        REQUIRE_THROWS_AS(hostapd.SetPairwiseCipherSuites({
            { WpaSecurityProtocol::Wpa, {} },
            { WpaSecurityProtocol::Wpa2, { WpaCipher::Aes128Cmac } },
        }, EnforceConfigurationChange::Defer), HostapdException);

        // Empty protocol list in map entry.
        REQUIRE_THROWS_AS(hostapd.SetPairwiseCipherSuites({
            { WpaSecurityProtocol::Wpa, { WpaCipher::Aes128Cmac } },
            { WpaSecurityProtocol::Wpa2, { } },
        }, EnforceConfigurationChange::Now), HostapdException);
        REQUIRE_THROWS_AS(hostapd.SetPairwiseCipherSuites({
            { WpaSecurityProtocol::Wpa, { WpaCipher::Aes128Cmac } },
            { WpaSecurityProtocol::Wpa2, { } },
        }, EnforceConfigurationChange::Defer), HostapdException);

        // Empty protocol list and empty cipher list in map entry.
        REQUIRE_THROWS_AS(hostapd.SetPairwiseCipherSuites({
            { WpaSecurityProtocol::Wpa, { WpaCipher::Aes128Cmac } },
            { {}, {} },
        }, EnforceConfigurationChange::Now), HostapdException);
        REQUIRE_THROWS_AS(hostapd.SetPairwiseCipherSuites({
            { WpaSecurityProtocol::Wpa, { WpaCipher::Aes128Cmac } },
            { {}, {} },
        }, EnforceConfigurationChange::Defer), HostapdException);

        // Only empty protocol and cipher lists in map.
        REQUIRE_THROWS_AS(hostapd.SetPairwiseCipherSuites({
            { {}, {} },
        }, EnforceConfigurationChange::Now), HostapdException);
        REQUIRE_THROWS_AS(hostapd.SetPairwiseCipherSuites({
            { {}, {} },
        }, EnforceConfigurationChange::Defer), HostapdException);
        // clang-format on
    }

    SECTION("Succeeds with all valid, single inputs")
    {
        for (const auto WpaSecurityProtocol : magic_enum::enum_values<WpaSecurityProtocol>() | std::views::filter(IsWpaSecurityProtocolSupported)) {
            for (const auto wpaCipher : WpaCiphersAll | std::views::filter(IsWpaCipherSupported)) {
                REQUIRE_NOTHROW(hostapd.SetPairwiseCipherSuites(WpaSecurityProtocol, { wpaCipher }, EnforceConfigurationChange::Now));
                REQUIRE_NOTHROW(hostapd.SetPairwiseCipherSuites(WpaSecurityProtocol, { wpaCipher }, EnforceConfigurationChange::Defer));
            }
        }
    }

    SECTION("Succeeds with valid, multiple inputs")
    {
        std::unordered_map<WpaSecurityProtocol, std::vector<WpaCipher>> protocolCipherMap{};

        for (const auto WpaSecurityProtocol : magic_enum::enum_values<WpaSecurityProtocol>() | std::views::filter(IsWpaSecurityProtocolSupported)) {
            for (const auto wpaCipher : WpaCiphersAll | std::views::filter(IsWpaCipherSupported)) {
                protocolCipherMap[WpaSecurityProtocol].push_back(wpaCipher);
                REQUIRE_NOTHROW(hostapd.SetPairwiseCipherSuites(protocolCipherMap, EnforceConfigurationChange::Now));
                REQUIRE_NOTHROW(hostapd.SetPairwiseCipherSuites(protocolCipherMap, EnforceConfigurationChange::Defer));
            }
        }
    }

    SECTION("Succeeds with valid, duplicate inputs")
    {
        REQUIRE_NOTHROW(hostapd.SetPairwiseCipherSuites(WpaSecurityProtocol::Wpa, { WpaCipher::Aes128Cmac, WpaCipher::Aes128Cmac }, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.SetPairwiseCipherSuites(WpaSecurityProtocol::Wpa, { WpaCipher::Aes128Cmac, WpaCipher::Aes128Cmac }, EnforceConfigurationChange::Defer));

        REQUIRE_NOTHROW(hostapd.SetPairwiseCipherSuites({ { WpaSecurityProtocol::Wpa, { WpaCipher::Aes128Cmac, WpaCipher::Aes128Cmac } } }, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.SetPairwiseCipherSuites({ { WpaSecurityProtocol::Wpa, { WpaCipher::Aes128Cmac, WpaCipher::Aes128Cmac } } }, EnforceConfigurationChange::Defer));
    }
}

TEST_CASE("Send SetAuthenticationAlgorithms() command (root)", "[wpa][hostapd][client][remote]")
{
    using namespace Wpa;

    Hostapd hostapd(WpaDaemonManager::InterfaceNameDefault);

    SECTION("Doesn't throw")
    {
        REQUIRE_NOTHROW(hostapd.SetAuthenticationAlgorithms({ WpaAuthenticationAlgorithm::OpenSystem }, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.SetAuthenticationAlgorithms({ WpaAuthenticationAlgorithm::OpenSystem }, EnforceConfigurationChange::Defer));
    }

    SECTION("Fails with empty input")
    {
        REQUIRE_THROWS_AS(hostapd.SetAuthenticationAlgorithms({}, EnforceConfigurationChange::Now), HostapdException);
        REQUIRE_THROWS_AS(hostapd.SetAuthenticationAlgorithms({}, EnforceConfigurationChange::Defer), HostapdException);
    }

    SECTION("Fails with invalid input")
    {
        for (const auto wpaAuthenticationAlgorithmUnsupported : WpaAuthenticationAlgorithmsUnsupported) {
            REQUIRE_THROWS_AS(hostapd.SetAuthenticationAlgorithms({ wpaAuthenticationAlgorithmUnsupported }, EnforceConfigurationChange::Now), HostapdException);
            REQUIRE_THROWS_AS(hostapd.SetAuthenticationAlgorithms({ wpaAuthenticationAlgorithmUnsupported }, EnforceConfigurationChange::Defer), HostapdException);
        }
    }

    SECTION("Succeeds with valid, single input")
    {
        for (const auto wpaAuthenticationAlgorithm : WpaAuthenticationAlgorithmsAll | std::views::filter(IsWpaAuthenticationAlgorithmSupported)) {
            REQUIRE_NOTHROW(hostapd.SetAuthenticationAlgorithms({ wpaAuthenticationAlgorithm }, EnforceConfigurationChange::Now));
            REQUIRE_NOTHROW(hostapd.SetAuthenticationAlgorithms({ wpaAuthenticationAlgorithm }, EnforceConfigurationChange::Defer));
        }
    }

    SECTION("Succeeds with valid, multiple inputs")
    {
        std::vector<WpaAuthenticationAlgorithm> wpaAuthenticationAlgorithms{};
        for (const auto wpaAuthenticationAlgorithm : WpaAuthenticationAlgorithmsAll | std::views::filter(IsWpaAuthenticationAlgorithmSupported)) {
            wpaAuthenticationAlgorithms.push_back(wpaAuthenticationAlgorithm);
            REQUIRE_NOTHROW(hostapd.SetAuthenticationAlgorithms(wpaAuthenticationAlgorithms, EnforceConfigurationChange::Now));
            REQUIRE_NOTHROW(hostapd.SetAuthenticationAlgorithms(wpaAuthenticationAlgorithms, EnforceConfigurationChange::Defer));
        }
    }

    SECTION("Succeeds with valid, duplicate inputs")
    {
        REQUIRE_NOTHROW(hostapd.SetAuthenticationAlgorithms({ WpaAuthenticationAlgorithm::OpenSystem, WpaAuthenticationAlgorithm::OpenSystem }, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.SetAuthenticationAlgorithms({ WpaAuthenticationAlgorithm::OpenSystem, WpaAuthenticationAlgorithm::OpenSystem }, EnforceConfigurationChange::Defer));
    }
}

namespace Wpa::Test
{
// NOLINTBEGIN(cert-err58-cpp)
// clang-format off
constexpr auto PskPassphraseValid{ "password" };
constexpr std::array<char, WpaPskValueLength> PskHexValid{ 
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
// clang-format on

constexpr auto AsciiPassword{ "password" };
constexpr auto PasswordIdValid{ "someid" };
constexpr auto PeerMacAddressValid{ "00:11:22:33:44:55" };
constexpr auto BridgeInterfaceNameDefault{ "brgateway0" };
constexpr int32_t VlanIdValid{ 1 };

const SaePassword SaePasswordValid1{
    .Credential = AsciiPassword,
    .PasswordId = std::nullopt,
    .PeerMacAddress = std::nullopt,
    .VlanId = std::nullopt,
};

const SaePassword SaePasswordValid2{
    .Credential = AsciiPassword,
    .PasswordId = PasswordIdValid,
    .PeerMacAddress = std::nullopt,
    .VlanId = std::nullopt,
};

const SaePassword SaePasswordValid3{
    .Credential = AsciiPassword,
    .PasswordId = PasswordIdValid,
    .PeerMacAddress = PeerMacAddressValid,
    .VlanId = std::nullopt,
};

const SaePassword SaePasswordValidComplete{
    .Credential = AsciiPassword,
    .PasswordId = PasswordIdValid,
    .PeerMacAddress = PeerMacAddressValid,
    .VlanId = VlanIdValid,
};

const std::vector<SaePassword> SaePasswordsValid{ SaePasswordValid1, SaePasswordValid2, SaePasswordValid3, SaePasswordValidComplete };

// NOLINTEND(cert-err58-cpp)
} // namespace Wpa::Test

TEST_CASE("Send SetPreSharedKey() command (root)", "[wpa][hostapd][client][remote]")
{
    using namespace Wpa;
    using namespace Wpa::Test;

    Hostapd hostapd(WpaDaemonManager::InterfaceNameDefault);

    SECTION("Doesn't throw")
    {
        REQUIRE_NOTHROW(hostapd.SetPreSharedKey(PskHexValid, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.SetPreSharedKey(PskHexValid, EnforceConfigurationChange::Defer));

        REQUIRE_NOTHROW(hostapd.SetPreSharedKey(PskPassphraseValid, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.SetPreSharedKey(PskPassphraseValid, EnforceConfigurationChange::Defer));
    }

    SECTION("Succeeds with passphrase inputs that are valid")
    {
        for (std::size_t i = WpaPskPassphraseLengthMin; i <= WpaPskPassphraseLengthMax; ++i) {
            const auto pskPassphrase = std::string(i, 'a');
            REQUIRE_NOTHROW(hostapd.SetPreSharedKey(pskPassphrase, EnforceConfigurationChange::Now));
            REQUIRE_NOTHROW(hostapd.SetPreSharedKey(pskPassphrase, EnforceConfigurationChange::Defer));
        }
    }

    SECTION("Fails with passphrase inputs that are too short")
    {
        for (std::size_t i = 0; i < WpaPskPassphraseLengthMin; ++i) {
            const auto pskPassphrase = std::string(i, 'a');
            REQUIRE_THROWS_AS(hostapd.SetPreSharedKey(pskPassphrase, EnforceConfigurationChange::Now), HostapdException);
            REQUIRE_THROWS_AS(hostapd.SetPreSharedKey(pskPassphrase, EnforceConfigurationChange::Defer), HostapdException);
        }
    }

    SECTION("Fails with passphrase input that is too long")
    {
        const auto pskPassphrase = std::string(WpaPskPassphraseLengthMax + 1, 'a');
        REQUIRE_THROWS_AS(hostapd.SetPreSharedKey(pskPassphrase, EnforceConfigurationChange::Now), HostapdException);
        REQUIRE_THROWS_AS(hostapd.SetPreSharedKey(pskPassphrase, EnforceConfigurationChange::Defer), HostapdException);
    }
}

TEST_CASE("Send AddSaePassword() command (root)", "[wpa][hostapd][client][remote]")
{
    using namespace Wpa;
    using namespace Wpa::Test;

    Hostapd hostapd(WpaDaemonManager::InterfaceNameDefault);

    SECTION("Doesn't throw")
    {
        REQUIRE_NOTHROW(hostapd.AddSaePassword(SaePasswordValidComplete, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.AddSaePassword(SaePasswordValidComplete, EnforceConfigurationChange::Defer));
    }

    SECTION("Succeeds with valid inputs")
    {
        for (const auto& saePassword : SaePasswordsValid) {
            REQUIRE_NOTHROW(hostapd.AddSaePassword(saePassword, EnforceConfigurationChange::Now));
            REQUIRE_NOTHROW(hostapd.AddSaePassword(saePassword, EnforceConfigurationChange::Defer));
        }
    }
}

TEST_CASE("Send SetSaePasswords() command (root)", "[wpa][hostapd][client][remote]")
{
    using namespace Wpa;
    using namespace Wpa::Test;

    Hostapd hostapd(WpaDaemonManager::InterfaceNameDefault);

    SECTION("Doesn't throw")
    {
        REQUIRE_NOTHROW(hostapd.SetSaePasswords({ SaePasswordValidComplete }, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.SetSaePasswords({ SaePasswordValidComplete }, EnforceConfigurationChange::Defer));
    }

    SECTION("Succeeds with valid inputs")
    {
        for (const auto& saePassword : SaePasswordsValid) {
            REQUIRE_NOTHROW(hostapd.SetSaePasswords({ saePassword }, EnforceConfigurationChange::Now));
            REQUIRE_NOTHROW(hostapd.SetSaePasswords({ saePassword }, EnforceConfigurationChange::Defer));
        }
    }
}

TEST_CASE("Send SetBridgeInterface() command (root)", "[wpa][hostapd][client][remote]")
{
    using namespace Wpa;
    using namespace Wpa::Test;

    Hostapd hostapd(WpaDaemonManager::InterfaceNameDefault);

    SECTION("Doesn't throw")
    {
        REQUIRE_NOTHROW(hostapd.SetBridgeInterface(BridgeInterfaceNameDefault, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.SetBridgeInterface(BridgeInterfaceNameDefault, EnforceConfigurationChange::Defer));
    }
}

TEST_CASE("Send AddRadiusEndpoints() command (root)", "[wpa][hostapd][client][remote]")
{
    using namespace Wpa;
    using namespace Wpa::Test;

    static constexpr auto RadiusServerIpValid1{ "127.0.0.1" };
    static constexpr auto RadiusServerIpValid2{ "192.168.0.1" };
    static constexpr auto RadiusServerPortValid1{ RadiusAuthenticationPortDefault };
    static constexpr auto RadiusServerPortValid2{ 1234 };
    static constexpr auto RadiusSecretValid1{ "shared-secret-1" };
    static constexpr auto RadiusSecretValid2{ "shared-secret-2" };

    static constexpr RadiusEndpointConfiguration RadiusEndpointConfigurationAuthenticationValid1{
        .Type = RadiusEndpointType::Authentication,
        .Address = RadiusServerIpValid1,
        .Port = RadiusServerPortValid1,
        .SharedSecret = RadiusSecretValid1,
    };
    static constexpr RadiusEndpointConfiguration RadiusEndpointConfigurationAuthenticationValid2{
        .Type = RadiusEndpointType::Authentication,
        .Address = RadiusServerIpValid2,
        .Port = RadiusServerPortValid2,
        .SharedSecret = RadiusSecretValid2,
    };

    static constexpr RadiusEndpointConfiguration RadiusEndpointConfigurationAccountingValid1{
        .Type = RadiusEndpointType::Accounting,
        .Address = RadiusServerIpValid2,
        .Port = RadiusServerPortValid2,
        .SharedSecret = RadiusSecretValid2,
    };
    static constexpr RadiusEndpointConfiguration RadiusEndpointConfigurationAccountingValid2{
        .Type = RadiusEndpointType::Accounting,
        .Address = RadiusServerIpValid2,
        .Port = RadiusServerPortValid2,
        .SharedSecret = RadiusSecretValid2,
    };

    Hostapd hostapd(WpaDaemonManager::InterfaceNameDefault);

    SECTION("Doesn't throw")
    {
        REQUIRE_NOTHROW(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationAuthenticationValid1 }, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationAuthenticationValid1 }, EnforceConfigurationChange::Defer));
    }

    SECTION("Fails with invalid endpoint type")
    {
        static constexpr RadiusEndpointConfiguration radiusEndpointConfigurationInvalid{
            .Type = RadiusEndpointType::Unknown,
            .Address = RadiusServerIpValid1,
            .SharedSecret = RadiusSecretValid1,
        };

        REQUIRE_THROWS_AS(hostapd.AddRadiusEndpoints({ radiusEndpointConfigurationInvalid }, EnforceConfigurationChange::Now), HostapdException);
        REQUIRE_THROWS_AS(hostapd.AddRadiusEndpoints({ radiusEndpointConfigurationInvalid }, EnforceConfigurationChange::Defer), HostapdException);
        REQUIRE_THROWS_AS(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationAuthenticationValid1, radiusEndpointConfigurationInvalid }, EnforceConfigurationChange::Now), HostapdException);
        REQUIRE_THROWS_AS(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationAuthenticationValid1, radiusEndpointConfigurationInvalid }, EnforceConfigurationChange::Defer), HostapdException);
    }

    SECTION("Fails with missing address input")
    {
        static constexpr RadiusEndpointConfiguration RadiusEndpointConfigurationInvalid{
            .Type = RadiusEndpointType::Authentication,
            .Address = "",
            .SharedSecret = RadiusSecretValid1,
        };

        REQUIRE_THROWS_AS(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationInvalid }, EnforceConfigurationChange::Now), HostapdException);
        REQUIRE_THROWS_AS(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationInvalid }, EnforceConfigurationChange::Defer), HostapdException);
        REQUIRE_THROWS_AS(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationAuthenticationValid1, RadiusEndpointConfigurationInvalid }, EnforceConfigurationChange::Now), HostapdException);
        REQUIRE_THROWS_AS(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationAuthenticationValid1, RadiusEndpointConfigurationInvalid }, EnforceConfigurationChange::Defer), HostapdException);
    }

    SECTION("Fails with missing shared secret input")
    {
        static constexpr RadiusEndpointConfiguration RadiusEndpointConfigurationInvalid{
            .Type = RadiusEndpointType::Authentication,
            .Address = RadiusServerIpValid1,
            .SharedSecret = "",
        };

        REQUIRE_THROWS_AS(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationInvalid }, EnforceConfigurationChange::Now), HostapdException);
        REQUIRE_THROWS_AS(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationInvalid }, EnforceConfigurationChange::Defer), HostapdException);
        REQUIRE_THROWS_AS(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationAuthenticationValid1, RadiusEndpointConfigurationInvalid }, EnforceConfigurationChange::Now), HostapdException);
        REQUIRE_THROWS_AS(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationAuthenticationValid1, RadiusEndpointConfigurationInvalid }, EnforceConfigurationChange::Defer), HostapdException);
    }

    SECTION("Succeeds with a port specified")
    {
        static constexpr RadiusEndpointConfiguration RadiusEndpointConfigurationValid{
            .Type = RadiusEndpointType::Authentication,
            .Address = RadiusServerIpValid1,
            .Port = RadiusServerPortValid1,
            .SharedSecret = RadiusSecretValid1,
        };

        REQUIRE_NOTHROW(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationValid }, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationValid }, EnforceConfigurationChange::Defer));
    }

    SECTION("Succeeds without a port specified")
    {
        REQUIRE_NOTHROW(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationAuthenticationValid1 }, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationAuthenticationValid1 }, EnforceConfigurationChange::Defer));
    }

    SECTION("Succeeds with a single authentication server specified")
    {
        REQUIRE_NOTHROW(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationAuthenticationValid1 }, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationAuthenticationValid1 }, EnforceConfigurationChange::Defer));
    }

    SECTION("Succeeds with multiple authentication servers specified")
    {
        REQUIRE_NOTHROW(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationAuthenticationValid1, RadiusEndpointConfigurationAuthenticationValid2 }, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationAuthenticationValid1, RadiusEndpointConfigurationAuthenticationValid2 }, EnforceConfigurationChange::Defer));
    }

    SECTION("Succeeds with a single accounting server specified")
    {
        REQUIRE_NOTHROW(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationAccountingValid1 }, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationAccountingValid1 }, EnforceConfigurationChange::Defer));
    }

    SECTION("Succeeds with multiple accounting servers specified")
    {
        REQUIRE_NOTHROW(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationAccountingValid1, RadiusEndpointConfigurationAccountingValid2 }, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationAccountingValid1, RadiusEndpointConfigurationAccountingValid2 }, EnforceConfigurationChange::Defer));
    }

    SECTION("Succeeds with single authentication and accounting servers specified")
    {
        REQUIRE_NOTHROW(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationAuthenticationValid1, RadiusEndpointConfigurationAccountingValid1 }, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationAuthenticationValid1, RadiusEndpointConfigurationAccountingValid1 }, EnforceConfigurationChange::Defer));
    }

    SECTION("Succeeds with mixtures of authentication and accounting servers specified")
    {
        REQUIRE_NOTHROW(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationAuthenticationValid1, RadiusEndpointConfigurationAuthenticationValid2, RadiusEndpointConfigurationAccountingValid1, RadiusEndpointConfigurationAccountingValid2 }, EnforceConfigurationChange::Now));
        REQUIRE_NOTHROW(hostapd.AddRadiusEndpoints({ RadiusEndpointConfigurationAuthenticationValid1, RadiusEndpointConfigurationAuthenticationValid2, RadiusEndpointConfigurationAccountingValid1, RadiusEndpointConfigurationAccountingValid2 }, EnforceConfigurationChange::Defer));
    }
}
