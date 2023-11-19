
#include <algorithm>
#include <cctype>
#include <iostream>
#include <optional>
#include <ranges>
#include <string>

#include <catch2/catch_test_case_info.hpp>
#include <catch2/interfaces/catch_interfaces_reporter.hpp>
#include <magic_enum.hpp>

#include "WpaDaemonCatch2EventListener.hxx"

namespace detail
{
/**
 * @brief Determines if the specified characters are equal, ignoring case.
 * 
 * @param lhs
 * @param rhs
 * @return true
 * @return false
 */
inline bool CaseInsensitiveCharEquals(char lhs, char rhs)
{
    return std::tolower(static_cast<unsigned char>(lhs)) == std::tolower(static_cast<unsigned char>(rhs));
}

/**
 * @brief Determines if the specified strings are equal, ignoring case.
 * 
 * @param lhs
 * @param rhs
 * @return true
 * @return false
 */
inline bool CaseInsensitiveStringEquals(std::string_view lhs, std::string_view rhs)
{
    return std::ranges::equal(lhs, rhs, CaseInsensitiveCharEquals);
}

/**
 * @brief Get the first wpa daemon type from the specified test tags.
 * 
 * This function searching the specified list of tags for the first tag which
 * matches the enum name of a WpaType. A case insensitive comparison is used to
 * allow the tags to be lower case, which is idiomatic for Catch2.
 * 
 * @param tags The list of test tags to search.
 * @return std::optional<Wpa::WpaType> 
 */
std::optional<Wpa::WpaType> GetWpaDaemonTypeFromTags([[maybe_unused]] const std::vector<Catch::Tag>& tags)
{
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
    std::cout << "Test case starting: " << testInfo.name << std::endl;

    // Determine if this test case has a tag corresponding to a wpa daemon type (WpaType).
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

