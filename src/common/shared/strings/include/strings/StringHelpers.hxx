
#ifndef STRING_HELPERS_HXX
#define STRING_HELPERS_HXX

#include <algorithm>
#include <cctype>
#include <concepts>
#include <cstddef>
#include <string>
#include <string_view>

namespace Strings
{
/**
 * @brief Cast a character for use in lowercase conversions.
 *
 * @tparam CharT The type to be cast from.
 * @param c The character value to cast.
 * @return
 */
// clang-format off
template <typename CharT>
requires std::integral<CharT>
// clang-format on
unsigned char
LowerCast(CharT c)
{
    return static_cast<unsigned char>(c);
}

/**
 * @brief std::tolower() analog which properly casts the input character to an
 * appropriate type.
 *
 * @param c
 * @return int
 */
template <typename CharT>
int
ToLower(CharT c)
{
    return std::tolower(LowerCast(c));
}

/**
 * @brief Converts the specified string to lowercase.
 *
 * @param s
 * @return std::string
 */
std::string
ToLower(std::string s);

/**
 * @brief Determines if the specified characters are equal, ignoring case.
 *
 * @param lhs
 * @param rhs
 * @return true
 * @return false
 */
bool
CaseInsensitiveCharEquals(char lhs, char rhs);

/**
 * @brief Determines if the specified strings are equal, ignoring case.
 *
 * @param lhs
 * @param rhs
 * @return true
 * @return false
 */
bool
CaseInsensitiveStringEquals(std::string_view lhs, std::string_view rhs);

/**
 * @brief Generate a random string with ASCII characters of the specified length.
 *
 * @param length The desired length of the string.
 * @return std::string
 */
std::string
GenerateRandomAsciiString(std::size_t length);

/**
 * @brief Helper to convert an arbitrary input to a string view.
 */
constexpr auto ToStringView = [](auto&& sv) {
    return std::string_view(sv);
};

} // namespace Strings

#endif // STRING_HELPERS_HXX
