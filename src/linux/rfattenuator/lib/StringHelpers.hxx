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

// /**
//  * @brief
//  * @brief Convert a wide string to a narrow string.
//  *
//  * @param ws The wide string to convert to a narrow string.
//  * @param codepage The codepage of the string characters.
//  * @return std::string
//  */
// std::string
// ToNarrow(const std::wstring& ws, unsigned int codepage = CP_UTF8);

// /**
//  * @brief Convert a wide string to a narrow string.
//  *
//  * @param ws The wide string to convert to a narrow string.
//  * @param length The number of characters in the string.
//  * @param codepage The codepage of the string characters.
//  * @return std::string
//  */
// std::string
// ToNarrow(const wchar_t* ws, std::size_t length, unsigned int codepage = CP_UTF8);

// /**
//  * @brief Convert a narrow string to a wide string.
//  *
//  * @param s The narrow string to convert.
//  * @param codepage The codepage of the string characters.
//  * @return std::wstring
//  */
// std::wstring
// ToWide(const std::string& s, unsigned int codepage = CP_UTF8);

// /**
//  * @brief
//  *
//  * @param s The narrow string to convert.
//  * @param length The number of characters in the string.
//  * @param codepage The codepage of the string characters.
//  * @return std::wstring
//  */
// std::wstring
// ToWide(const char* s, std::size_t length, unsigned int codepage = CP_UTF8);
} // namespace Strings
