
#include <algorithm>
#include <cctype>
#include <format>
#include <iostream>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <string>

#include <catch2/catch_test_case_info.hpp>
#include <catch2/interfaces/catch_interfaces_reporter.hpp>
#include <magic_enum.hpp>
#include <strings/StringHelpers.hxx>

#include "WpaDaemonCatch2EventListener.hxx"

namespace detail
{
/**
 * @brief Get the first wpa daemon type from the specified test tags.
 * 
 * This function searching the specified list of tags for the first tag which
 * matches the enum name of a WpaType. A case insensitive comparison is used to
 * allow the tags to be lower case, which is idiomatic for Catch2.
 * 
 * TODO: change this to return a vector or unordered_set of WpaType values.
 * 
 * @param tags The list of test tags to search.
 * @return std::optional<Wpa::WpaType> 
 */
std::optional<Wpa::WpaType> GetWpaDaemonTypeFromTags(const std::vector<Catch::Tag>& tags)
{
    using Strings::CaseInsensitiveStringEquals;

    // Convert the tag vector into strings so they can be compared to the
    // WpaType enum names. The Catch::Tag implementation uses their own string
    // type and does not provide an implicit conversion to std::string, so the
    // explicit conversion operator is used directly here.
    const auto wpaNames = magic_enum::enum_names<Wpa::WpaType>();
    const auto tagNames = tags | std::views::transform([](const auto& tag){
        return tag.original.operator std::string();
    });

    // Find the first entry which matches the tag name, ignoring case.
    const auto result = std::ranges::find_first_of(wpaNames, tagNames, CaseInsensitiveStringEquals);
    if (result != std::cend(wpaNames))
    {
        return magic_enum::enum_cast<Wpa::WpaType>(*result).value();
    }

    return std::nullopt;
}

} // namespace detail

void WpaDaemonCatch2EventListener::testCaseStarting(Catch::TestCaseInfo const& testInfo)
{
    // Determine if this test case has a tag corresponding to a wpa daemon type (WpaType).
    const auto wpaType = detail::GetWpaDaemonTypeFromTags(testInfo.tags);
    if (!wpaType.has_value())
    {
        std::cout << "Test case does not contain WpaType tag." << std::endl;
        return;
    }

    const auto wpaDaemon = Wpa::GetWpaTypeDaemonBinaryName(wpaType.value());

    std::cout << std::format("Test case for {} daemon starting\n", wpaDaemon);

    // Remove any existing mac80211_hwsim virtual wifi devices for the test case.
    const auto driverMac8021Hwsim = WifiVirtualDeviceManager::DriverMac80211HwsimName;
    const auto driverRemoved = m_wifiVirtualDeviceManager.RemoveInterfaces(driverMac8021Hwsim);
    if (!driverRemoved)
    {
        throw std::runtime_error(std::format("Failed to remove {} virtual wifi devices for test case.", driverMac8021Hwsim));
    }

    // Create 1 mac80211_hwsim virtualized wifi device for the test case.
    const auto wifiDevices = m_wifiVirtualDeviceManager.CreateInterfaces(1, WifiVirtualDeviceManager::DriverMac80211HwsimName);
    if (std::empty(wifiDevices))
    {
        throw std::runtime_error(std::format("Failed to create {} virtual wifi device for test case.", driverMac8021Hwsim));
    }

    const auto& interfaceName = *std::cbegin(wifiDevices);
    std::cout << std::format("Created {} virtual wifi device {}\n", driverMac8021Hwsim, interfaceName);

    // Start the wpa daemon instance for the test case.
    std::unique_ptr<IWpaDaemonInstance> wpaDaemonInstance;
    switch (wpaType.value())
    {
    case Wpa::WpaType::Hostapd:
        wpaDaemonInstance = std::make_unique<WpaDaemonInstance<Wpa::WpaType::Hostapd>>();
        break;
    case Wpa::WpaType::WpaSupplicant:
        wpaDaemonInstance = std::make_unique<WpaDaemonInstance<Wpa::WpaType::WpaSupplicant>>();
        break;
    default:
        break;
    }

    if (wpaDaemonInstance == nullptr)
    {
        throw std::runtime_error(std::format("Failed to create wpa {} daemon instance", wpaDaemon));
    }

    wpaDaemonInstance->OnStarting();
    m_wpaDaemonInstances.emplace(wpaType.value(), std::move(wpaDaemonInstance));

    std::cout << std::format("Started wpa {} daemon instance\n", wpaDaemon);
}

void WpaDaemonCatch2EventListener::testCaseEnded(Catch::TestCaseStats const& testCaseStats)
{
    const auto wpaType = detail::GetWpaDaemonTypeFromTags(testCaseStats.testInfo->tags);
    if (!wpaType.has_value())
    {
        std::cout << "Test case does not contain WpaType tag." << std::endl;
        return;
    }

    const auto wpaDaemon = Wpa::GetWpaTypeDaemonBinaryName(wpaType.value());

    std::cout << std::format("Test case for {} daemon stopping\n", wpaDaemon);
    auto wpaDaemonInstanceNode = m_wpaDaemonInstances.extract(wpaType.value());
    if (wpaDaemonInstanceNode.empty())
    {
        std::cout << std::format("Test case for {} daemon does not have an instance.", wpaDaemon) << std::endl;
        return;
    }

    auto wpaDaemonInstance = std::move(wpaDaemonInstanceNode.mapped());
    wpaDaemonInstance->OnStopping();
    wpaDaemonInstance.reset();
}
