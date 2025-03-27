
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

// std::string
// ToNarrow(const std::wstring& ws, unsigned int codepage)
// {
//     return ToNarrow(ws.c_str(), std::size(ws), codepage);
// }

// std::string
// ToNarrow(const wchar_t* ws, std::size_t length, unsigned int codepage)
// {
//     // Easy case: empty string.
//     if (length == 0) {
//         return {};
//     }

//     // Determine the size of the converted string.
//     auto convertedLength = WideCharToMultiByte(codepage, 0, ws, static_cast<int>(length), nullptr, 0, nullptr, nullptr);
//     THROW_LAST_ERROR_IF(convertedLength <= 0);

//     // Allocate a terminated string with the required size.
//     std::string narrow(convertedLength, '\0');

//     // Convert the string, using the coverted string buffer directly.
//     convertedLength = WideCharToMultiByte(codepage, 0, ws, static_cast<int>(length), &narrow[0], convertedLength, nullptr, nullptr);
//     THROW_LAST_ERROR_IF(convertedLength <= 0);

//     return narrow;
// }

// std::wstring
// ToWide(const std::string& s, unsigned int codepage)
// {
//     return ToWide(s.c_str(), std::size(s), codepage);
// }

// std::wstring
// ToWide(const char* s, std::size_t length, unsigned int codepage)
// {
//     // Easy case: empty string.
//     if (length == 0) {
//         return {};
//     }

//     // Determine the size of the converted string.
//     auto convertedLength = MultiByteToWideChar(codepage, 0, s, static_cast<int>(length), nullptr, 0);
//     THROW_LAST_ERROR_IF(convertedLength <= 0);

//     // Allocate a terminated string with the required size.
//     std::wstring wide(convertedLength, '\0');

//     // Convert the string, using the converted string buffer directly.
//     convertedLength = MultiByteToWideChar(codepage, 0, s, static_cast<int>(length), &wide[0], convertedLength);
//     THROW_LAST_ERROR_IF(convertedLength <= 0);

//     return wide;
// }

} // namespace Strings
