
#include <format>
#include <initializer_list>
#include <memory>
#include <string_view>
#include <unordered_map>

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
    if (std::empty(m_propertiesToParse)) {
        return true;
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
