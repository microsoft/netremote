
#include <format>
#include <iterator>
#include <optional>
#include <string_view>

#include <Wpa/WpaKeyValuePair.hxx>
#include <plog/Log.h>

using namespace Wpa;

std::optional<std::string_view>
WpaKeyValuePair::TryParseValue(std::string_view input)
{
    // If not already parsed...
    if (!Value.has_value()) {
        // Find the starting position of the key.
        const auto keyPosition = input.find(Key);
        if (keyPosition != std::string_view::npos) {
            // Assign the starting position of the value, advancing past the key.
            input = input.substr(keyPosition + std::size(Key));

            // Find the end of the line, marking the end of the value string.
            const auto eolPosition = input.find('\n');
            if (eolPosition != std::string_view::npos) {
                Value = input.substr(0, eolPosition);
            } else {
                LOGW << std::format("Failed to parse value for key: {} (missing eol)", Key);
            }
        }
    }

    return Value;
}
