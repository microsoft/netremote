
#include <filesystem>
#include <memory>
#include <optional>
#include <string_view>

#include <Wpa/WpaCommand.hxx>
#include <Wpa/WpaResponse.hxx>
#include <Wpa/WpaController.hxx>
#include <Wpa/WpaCore.hxx>
#include <catch2/catch_test_macros.hpp>
#include <magic_enum.hpp>

#include "detail/WpaDaemonManager.hxx"

namespace TestDetail
{
constexpr auto WpaTypesSupported = {
    Wpa::WpaType::Hostapd,
};
} // namespace TestDetail

TEST_CASE("Send/receive WpaController request/response (root)", "[wpa][hostapd][client][remote]")
{
    using namespace TestDetail;
    using namespace Wpa;

    SECTION("Send/receive WpaCommand/WpaResponse")
    {
        for (const auto& wpaType : TestDetail::WpaTypesSupported) {
            WpaController wpaController(WpaDaemonManager::InterfaceNameDefault, wpaType);

            const WpaCommand wpaCommand("PING");
            std::shared_ptr<WpaResponse> wpaResponse;
            REQUIRE_NOTHROW(wpaResponse = wpaController.SendCommand(wpaCommand));
            REQUIRE(wpaResponse != nullptr);
            REQUIRE(wpaResponse->Payload().starts_with("PONG"));
        }
    }
}

TEST_CASE("Create a WpaController", "[wpa][client][local]")
{
    using namespace TestDetail;
    using namespace Wpa;

    const auto ControlSocketPath = std::filesystem::path(WpaDaemonManager::ControlSocketPathBase) / WpaDaemonManager::InterfaceNameDefault;
    const auto ControlSocketPathString = ControlSocketPath.string();
    const auto ControlSocketPathStringView = std::string_view(ControlSocketPathString);

    SECTION("Create for each daemon type doesn't cause a crash")
    {
        std::optional<WpaController> wpaController;
        for (const auto& wpaType : magic_enum::enum_values<WpaType>()) {
            REQUIRE_NOTHROW(wpaController.emplace(WpaDaemonManager::InterfaceNameDefault, wpaType));
        }
    }

    SECTION("Create for each daemon type reflects correct type")
    {
        for (const auto& wpaType : magic_enum::enum_values<WpaType>()) {
            const WpaController wpaController(WpaDaemonManager::InterfaceNameDefault, wpaType);
            REQUIRE(wpaController.Type() == wpaType);
        }
    }

    SECTION("Create with specific control socket path encoded as std::filesystem::path doesn't cause a crash")
    {
        std::optional<WpaController> wpaController;
        for (const auto& wpaType : magic_enum::enum_values<WpaType>()) {
            REQUIRE_NOTHROW(wpaController.emplace(WpaDaemonManager::InterfaceNameDefault, wpaType, ControlSocketPath));
        }
    }

    SECTION("Create with specific control socket path encoded as std::string_view doesn't cause a crash")
    {
        std::optional<WpaController> wpaController;
        for (const auto& wpaType : magic_enum::enum_values<WpaType>()) {
            REQUIRE_NOTHROW(wpaController.emplace(WpaDaemonManager::InterfaceNameDefault, wpaType, ControlSocketPathStringView));
        }
    }

    SECTION("Create with default control socket path doesn't cause a crash")
    {
        std::optional<WpaController> wpaController;
        for (const auto& wpaType : magic_enum::enum_values<WpaType>()) {
            REQUIRE_NOTHROW(wpaController.emplace(WpaDaemonManager::InterfaceNameDefault, wpaType));
        }
    }

    SECTION("Create with specific control socket path reflects correct path")
    {
        for (const auto& wpaType : magic_enum::enum_values<WpaType>()) {
            // Create with control socket path encoded as std::filesystem::path.
            const WpaController wpaController(WpaDaemonManager::InterfaceNameDefault, wpaType, ControlSocketPath);
            REQUIRE(wpaController.ControlSocketPath() == ControlSocketPath);
        }
    }
}
