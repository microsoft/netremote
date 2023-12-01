
#include <format>
#include <stdexcept>

#include <Wpa/WpaKeyValuePair.hxx>

using namespace Wpa;

std::optional<std::string_view> WpaKeyValuePair::TryParseValue(std::string_view input)
{
    // If not already parsed...
    if (!Value.has_value())
    {
        // Find the starting position of the key.
        const auto keyPosition = input.find(Key);
        if (keyPosition != input.npos)
        {
            // Assign the starting position of the value, advancing past the key.
            Value = std::data(input) + keyPosition + std::size(Key);
        }
    }

    return Value;
}
