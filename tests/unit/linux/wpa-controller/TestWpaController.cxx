
#include <memory>
#include <optional>

#include <catch2/catch_test_macros.hpp>
#include <magic_enum.hpp>

#include <Wpa/WpaController.hxx>

TEST_CASE("Create a WpaController", "[wpa][client][local]")
{
    static constexpr auto InterfaceNameDefault = "wlan0";
    static constexpr auto ControlSocketPathBase = "/run/";

    const auto ControlSocketPath = std::filesystem::path(ControlSocketPathBase) / InterfaceNameDefault;
    const auto ControlSocketPathString = ControlSocketPath.string();
    const auto ControlSocketPathStringView = std::string_view(ControlSocketPathString);

    using namespace Wpa;

    SECTION("Create for each daemon type doesn't cause a crash")
    {
        std::optional<WpaController> wpaController;
        for (const auto& wpaType : magic_enum::enum_values<WpaType>())
        {
            REQUIRE_NOTHROW(wpaController.emplace(InterfaceNameDefault, wpaType));
        }
    }

    SECTION("Create for each daemon type reflects correct type")
    {
        for (const auto& wpaType : magic_enum::enum_values<WpaType>())
        {
            WpaController wpaController(InterfaceNameDefault, wpaType);
            REQUIRE(wpaController.Type() == wpaType);
        }
    }

    SECTION("Create with specific control socket path doesn't cause a crash")
    {
        std::optional<WpaController> wpaController;
        for (const auto& wpaType : magic_enum::enum_values<WpaType>())
        {
            // Create with control socket path encoded as std::filesystem::path.
            REQUIRE_NOTHROW(wpaController.emplace(InterfaceNameDefault, wpaType, ControlSocketPath));
            // Create with control socket path encoded as std::string_view.
            REQUIRE_NOTHROW(wpaController.emplace(InterfaceNameDefault, wpaType, ControlSocketPathStringView));
        }
    }

    SECTION("Create with default control socket path doesn't cause a crash")
    {
        std::optional<WpaController> wpaController;
        for (const auto& wpaType : magic_enum::enum_values<WpaType>())
        {
            REQUIRE_NOTHROW(wpaController.emplace(InterfaceNameDefault, wpaType));
        }
    }

    SECTION("Create with specific control socket path reflects correct path")
    {
        for (const auto& wpaType : magic_enum::enum_values<WpaType>())
        {
            // Create with control socket path encoded as std::filesystem::path.
            WpaController wpaController(InterfaceNameDefault, wpaType, ControlSocketPath);
            REQUIRE(wpaController.ControlSocketPath() == ControlSocketPath);
        }
    }
}
