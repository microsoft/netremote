
#include "StringHelpers.hxx"

#include <algorithm>
#include <cctype>
#include <ranges>
#include <string>

namespace detail
{
bool
CaseInsensitiveCharEquals(char c1, char c2)
{
    return std::tolower(static_cast<unsigned char>(c1)) == std::tolower(static_cast<unsigned char>(c2));
}
} // namespace detail

namespace Strings
{
bool
IEquals(std::string_view s1, std::string_view s2)
{
    return std::ranges::equal(s1, s2, detail::CaseInsensitiveCharEquals);
}

std::string
ToLower(std::string s)
{
    std::transform(std::cbegin(s), std::cend(s), std::begin(s), [](int c) {
        return static_cast<unsigned char>(std::tolower(c));
    });

    return s;
}
} // namespace Strings
