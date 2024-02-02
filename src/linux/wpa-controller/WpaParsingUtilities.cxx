
#include <charconv>
#include <format>

#include <plog/Log.h>

#include "WpaParsingUtilities.hxx"

namespace Wpa::Parsing
{
bool
ParseInt(std::string_view value, int& valueInt) noexcept
{
    const auto [ptr, ec] = std::from_chars(std::data(value), std::data(value) + std::size(value), valueInt);
    if (ec != std::errc() || ptr != std::data(value) + std::size(value)) {
        LOGE << std::format("Failed to parse integer value: '{}'", value);
        return false;
    }

    return true;
}
} // namespace Wpa::Parsing
