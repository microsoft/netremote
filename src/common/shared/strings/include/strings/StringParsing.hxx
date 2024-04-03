
#ifndef STRING_PARSING_HXX
#define STRING_PARSING_HXX

#include <charconv>
#include <concepts>
#include <cstdint>
#include <cstdlib>
#include <ranges>
#include <string>
#include <tuple>

namespace Strings
{
/**
 * @brief Parse a hex string into a container of bytes.
 *
 * @tparam ContainerT The container type to store the parsed bytes.
 * @param hexString The hex string to parse.
 * @param result The container to store the parsed bytes.
 * @param numberOfBytesToParse The number of bytes to parse from the hex string.
 * @return true If a valid hex string was parsed into the container.
 * @return false If the hex string was invalid or the container was too small to store the parsed bytes.
 */
// clang-format off
template <std::ranges::range ContainerT>
requires std::same_as<std::ranges::range_value_t<ContainerT>, std::uint8_t>
// clang-format on
bool
ParseHexString(const std::string& hexString, ContainerT& result, std::size_t numberOfBytesToParse = std::tuple_size_v<ContainerT>)
{
    // Ensure the input has enough characters to parse the requested number of bytes.
    if (std::size(hexString) / 2 < numberOfBytesToParse) {
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
