#pragma once

#include <string>
#include <string_view>

namespace Strings
{
/**
 * @brief Case insensitive string comparison function.
 *
 * @param s1 First string to compare.
 * @param s2 Second string to compare.
 * @return true
 * @return false
 */
bool
IEquals(std::string_view s1, std::string_view s2);

/**
 * @brief Return a copy of the input string as lowercase.
 *
 * @param s The string to convert to lowercase.
 * @return std::string
 */
std::string
ToLower(std::string s);
} // namespace Strings
