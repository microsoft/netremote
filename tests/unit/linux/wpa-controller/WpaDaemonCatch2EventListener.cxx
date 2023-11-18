
#include <iostream>
#include <optional>

#include <catch2/catch_test_case_info.hpp>
#include <catch2/interfaces/catch_interfaces_reporter.hpp>
#include <magic_enum.hpp>

#include "WpaDaemonCatch2EventListener.hxx"

namespace detail
{
std::optional<Wpa::WpaType> GetWpaDaemonTypeFromTags([[maybe_unused]] const std::vector<Catch::Tag> tags)
{
    // TODO: Implement this function.
    return std::nullopt;
}

} // namespace detail

void WpaDaemonCatch2EventListener::testCaseStarting(Catch::TestCaseInfo const& testInfo)
{
    std::cout << "Test case starting: " << testInfo.name << std::endl;

    const auto wpaType = detail::GetWpaDaemonTypeFromTags(testInfo.tags);
    if (!wpaType.has_value())
    {
        std::cout << "Test case does not contain WpaType tag." << std::endl;
        return;
    }

    const auto wpaTypeName = magic_enum::enum_name(wpaType.value());

    std::cout << "Test case for {} daemon starting" << wpaTypeName << std::endl;

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

    if (wpaDaemonInstance != nullptr)
    {
        wpaDaemonInstance->OnStarting();
        m_wpaDaemonInstances.emplace(wpaType.value(), std::move(wpaDaemonInstance));
    }
}

void WpaDaemonCatch2EventListener::testCaseEnded(Catch::TestCaseStats const& testCaseStats)
{
    std::cout << "Test case ended: " << testCaseStats.testInfo->name << std::endl;

    const auto wpaType = detail::GetWpaDaemonTypeFromTags(testCaseStats.testInfo->tags);
    if (!wpaType.has_value())
    {
        std::cout << "Test case does not contain WpaType tag." << std::endl;
        return;
    }

    const auto wpaTypeName = magic_enum::enum_name(wpaType.value());

    std::cout << "Test case for {} daemon stopping" << wpaTypeName << std::endl;
    auto wpaDaemonInstanceNode = m_wpaDaemonInstances.extract(wpaType.value());
    if (wpaDaemonInstanceNode.empty())
    {
        std::cout << "Test case for {} daemon does not have an instance." << wpaTypeName << std::endl;
        return;
    }

    auto wpaDaemonInstance = std::move(wpaDaemonInstanceNode.mapped());
    wpaDaemonInstance->OnStopping();
    wpaDaemonInstance.reset();
}

