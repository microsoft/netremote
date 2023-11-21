
#ifndef STRING_CONVERSION_HXX
#define STRING_CONVERSION_HXX

#include <algorithm>
#include <cctype>
#include <string_view>
#include <string>

namespace Strings
{
/**
 * @brief Determines if the specified characters are equal, ignoring case.
 * 
 * @param lhs
 * @param rhs
 * @return true
 * @return false
 */
bool CaseInsensitiveCharEquals(char lhs, char rhs)
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
bool CaseInsensitiveStringEquals(std::string_view lhs, std::string_view rhs)
{
    return std::ranges::equal(lhs, rhs, CaseInsensitiveCharEquals);
}
} // namespace Strings

#endif // STRING_CONVERSION_HXX
