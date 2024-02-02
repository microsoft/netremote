
#ifndef WPA_PARSING_UTILITIES_HXX
#define WPA_PARSING_UTILITIES_HXX

#include <string_view>

namespace Wpa::Parsing
{
/**
 * @brief Parse a string into an integer.
 * 
 * @param value The string to parse.
 * @param valueInt The integer to store the result in.
 * @return true 
 * @return false 
 */
bool
ParseInt(std::string_view value, int& valueInt) noexcept;

} // namespace Wpa::Parsing

#endif // WPA_PARSING_UTILITIES_HXX
