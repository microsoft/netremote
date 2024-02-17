
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

    if (std::empty(m_propertiesToParse)) {
        return true;
    }

    // Parse the payload into individual lines containing key value pairs.
    auto lines = m_responsePayload | std::views::split(ProtocolWpa::KeyValueLineDelimeter) | std::views::transform(toStringView);

    // Parse each line into a key-value pair, and populate a map with them.
    std::unordered_map<std::string_view, std::string_view> properties;
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
        if (!std::empty(key)) {
            properties[key] = value;
            LOGV << std::format("[{}] {}={}", std::size(properties), key, value);
        }
    }

    for (auto propertyToParseIterator = std::begin(m_propertiesToParse); propertyToParseIterator != std::end(m_propertiesToParse);) {
        auto& propertyToParse = *propertyToParseIterator;
        auto propertyValue = propertyToParse.TryParseValue(m_responsePayload);
        if (propertyValue.has_value()) {
            m_properties[propertyToParse.Key] = *propertyValue;
            propertyToParseIterator = m_propertiesToParse.erase(propertyToParseIterator);
            continue;
        }
        if (propertyToParse.IsRequired) {
            LOGE << std::format("Failed to parse required property: {}\nPayload {}\n", propertyToParse.Key, m_responsePayload);
            return false;
        }
        ++propertyToParseIterator;
    }

    return true;
}
