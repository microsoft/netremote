
#include <format>
#include <iostream>

#include <Wpa/WpaResponseParser.hxx>

using namespace Wpa;

WpaResponseParser::WpaResponseParser(const WpaCommand* command, std::string_view responsePayload, std::initializer_list<WpaKeyValuePair> propertiesToParse) :
    Command(command),
    ResponsePayload(responsePayload),
    m_propertiesToParse(propertiesToParse)
{
}

const std::unordered_map<std::string_view, std::string_view>& WpaResponseParser::GetProperties() const noexcept
{
    return m_properties;
}

std::shared_ptr<WpaResponse> WpaResponseParser::Parse()
{
    // Attempt to parse the properties, bailing on an error.
    auto propertiesParsed = TryParseProperties();
    if (!propertiesParsed)
    {
        return nullptr;
    }

    // Parse the payload with the resolved properties.
    return ParsePayload();
}

bool WpaResponseParser::TryParseProperties()
{
    if (std::empty(m_propertiesToParse))
    {
        return true;
    }

    for (auto propertyToParseIterator = std::begin(m_propertiesToParse); propertyToParseIterator != std::end(m_propertiesToParse); )
    {
        auto& propertyToParse = *propertyToParseIterator;
        auto propertyValue = propertyToParse.TryParseValue(ResponsePayload);
        if (propertyValue.has_value())
        {
            m_properties[propertyToParse.Key] = *propertyValue;
            propertyToParseIterator = m_propertiesToParse.erase(propertyToParseIterator);
            continue;
        }
        else if (propertyToParse.IsRequired)
        {
            std::cerr << std::format("Failed to parse required property: {}", propertyToParse.Key) << std::endl;
            return false;
        }
        else
        {
            ++propertyToParseIterator;
        }
    }

    return true;
}
