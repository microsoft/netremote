
#include <algorithm>
#include <format>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <ranges>
#include <string_view>
#include <unordered_map>

#include <Wpa/ProtocolWpa.hxx>
#include <Wpa/WpaKeyValuePair.hxx>
#include <Wpa/WpaResponse.hxx>
#include <Wpa/WpaResponseParser.hxx>
#include <plog/Log.h>

using namespace Wpa;

WpaResponseParser::WpaResponseParser(const WpaCommand* command, std::string_view responsePayload, std::initializer_list<WpaKeyValuePair> propertiesToParse) :
    m_command(command),
    m_responsePayload(responsePayload),
    m_propertiesToParse(propertiesToParse)
{
}

const std::unordered_map<std::string_view, std::string_view>&
WpaResponseParser::GetProperties() const noexcept
{
    return m_properties;
}

std::shared_ptr<WpaResponse>
WpaResponseParser::Parse()
{
    // Attempt to parse the properties, bailing on an error.
    auto propertiesParsed = TryParseProperties();
    if (!propertiesParsed) {
        return nullptr;
    }

    // Parse the payload with the resolved properties.
    return ParsePayload();
}

const WpaCommand*
WpaResponseParser::GetCommand() const noexcept
{
    return m_command;
}

std::string_view
WpaResponseParser::GetResponsePayload() const noexcept
{
    return m_responsePayload;
}

bool
WpaResponseParser::TryParseProperties()
{
    // Convert a range to a string-view for pre-C++23 where std::string_view doesn't have a range constructor.
    constexpr auto toStringView = [](auto&& sv) {
        return std::string_view{ std::data(sv), std::size(sv) };
    };
    // Convert a key-value pair to its key.
    constexpr auto toKey = [](auto&& keyValuePair) {
        return keyValuePair.Key;
    };
    // Determine if the property map has the specified key.
    const auto hasKey = [&](auto&& key) {
        return m_properties.contains(key);
    };

    if (std::empty(m_propertiesToParse)) {
        return true;
    }

    // Parse the payload into individual lines containing key value pairs.
    auto lines = m_responsePayload | std::views::split(ProtocolWpa::KeyValueLineDelimeter) | std::views::transform(toStringView);

    // Parse each line into a key-value pair, and populate the property map with them.
    for (const auto line : lines) {
        auto keyValuePair = line | std::views::split(ProtocolWpa::KeyValueDelimiter) | std::views::transform(toStringView);
        auto keyValuePairIterator = std::begin(keyValuePair);

        if (keyValuePairIterator == std::end(keyValuePair)) {
            LOGV << std::format("Skipping empty key value pair line '{}'", line);
            continue;
        }

        // Ensure a delimited key and value were found.
        const auto numElements = std::ranges::distance(keyValuePairIterator, std::end(keyValuePair));
        if (numElements < 2) {
            LOGW << std::format("Skipping key value pair line '{}' with less than 2 elements ({})", line, numElements);
            continue;
        }

        // Parse out the key and value, ensuring the key is non-empty (empty values are allowed).
        const auto key = *std::next(keyValuePairIterator, 0);
        const auto value = *std::next(keyValuePairIterator, 1);
        if (std::empty(key)) {
            continue;
        }

        m_properties[key] = value;
        LOGV << std::format("Parsed [{:03}] {}={}", std::size(m_properties), key, value);
    }

    // Remove parsed properties from the list of properties to parse.
    m_propertiesToParse.erase(std::begin(std::ranges::remove_if(m_propertiesToParse, hasKey, toKey)), std::end(m_propertiesToParse));

    return true;
}
