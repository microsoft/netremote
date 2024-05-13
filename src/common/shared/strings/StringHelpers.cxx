
#include <algorithm>
#include <cstddef>
#include <random>
#include <ranges>
#include <string>
#include <string_view>

#include <strings/StringHelpers.hxx>

namespace Strings
{
std::string
ToLower(std::string s)
{
    std::ranges::transform(s, std::begin(s), [](auto c) {
        return ToLower(c);
    });

    return s;
}

bool
CaseInsensitiveCharEquals(char lhs, char rhs)
{
    return ToLower(lhs) == ToLower(rhs);
}

bool
CaseInsensitiveStringEquals(std::string_view lhs, std::string_view rhs)
{
    return std::ranges::equal(lhs, rhs, CaseInsensitiveCharEquals);
}

std::string
GenerateRandomAsciiString(std::size_t length)
{
    std::random_device randomNumberDevice;
    std::mt19937 randomNumberEngine(randomNumberDevice());
    std::uniform_int_distribution<int> randomNumberDistribution(' ', '~'); // ASCII range for printable characters (32-126).

    std::string result(length, '\0');
    std::ranges::generate_n(std::begin(result), static_cast<long>(length), [&]() {
        return static_cast<char>(randomNumberDistribution(randomNumberEngine));
    });

    return result;
}
} // namespace Strings
