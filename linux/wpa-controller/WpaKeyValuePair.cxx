
#include <format>
#include <stdexcept>

#include <notstd/Utility.hxx>

#include <Wpa/WpaKeyValuePair.hxx>

using namespace Wpa;

constexpr WpaKeyValuePair::WpaKeyValuePair(std::string_view key, WpaValuePresence presence) :
    Key(key),
    IsRequired(notstd::to_underlying(presence))
{
    // Ensure the specified key ends with the delimiter. If not, this is a
    // programming error so throw a runtime exception.
    if (!Key.ends_with(KeyDelimiter))
    {
        throw std::runtime_error(std::format("Key must end with {} delimiter.", KeyDelimiter));
    }
}

bool WpaKeyValuePair::TryParseValue(std::string_view input)
{
    // Find the starting position of the key.
    const auto keyPosition = input.find(Key);
    if (keyPosition == input.npos)
    {
        return false;
    }

    // Assign the starting position of the value, advancing past the key.
    Value = std::data(input) + keyPosition + std::size(Key);

    return true;
}
