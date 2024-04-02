
#ifndef STRING_PARSING_HXX
#define STRING_PARSING_HXX

#include <charconv>
#include <concepts>
#include <cstdint>
#include <cstdlib>
#include <ranges>
#include <string>

namespace Strings
{
/**
 * @brief Parse a hex string into a container of bytes.
 *
 * @tparam ContainerT The container type to store the parsed bytes.
 * @param hexString The hex string to parse.
 * @param result The container to store the parsed bytes.
 * @return true If a valid hex string was parsed into the container.
 * @return false If the hex string was invalid or the container was too small to store the parsed bytes.
 */
// clang-format off
template <std::ranges::range ContainerT>
requires std::same_as<std::ranges::range_value_t<ContainerT>, std::uint8_t>
// clang-format on
bool
ParseHexString(const std::string& hexString, ContainerT& result)
{
    // Ensure the result container is large enough to hold the parsed data. Since each byte is represented by 2 hex
    // characters, the result container must be at least half the size of the hex string.
    if (std::size(hexString) > std::size(result) * 2) {
        return false;
    }

    std::string_view hexStringView{ hexString };
    for (std::size_t i = 0; i < std::size(result); i++) {
        const auto byteAsHex = hexStringView.substr(i * 2, 2); // 2 hex chars
        const auto byteConversionResult = std::from_chars(std::data(byteAsHex), std::data(byteAsHex) + std::size(byteAsHex), result[i], 16);
        if (byteConversionResult.ec != std::errc{}) {
            return false;
        }
    }

    return true;
}
} // namespace Strings

#endif // STRING_PARSING_HXX
