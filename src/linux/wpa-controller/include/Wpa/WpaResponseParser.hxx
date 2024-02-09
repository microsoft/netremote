
#ifndef WPA_RESPONSE_PARSER_HXX
#define WPA_RESPONSE_PARSER_HXX

#include <initializer_list>
#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <Wpa/WpaKeyValuePair.hxx>
#include <Wpa/WpaResponse.hxx>

namespace Wpa
{
struct WpaCommand;

/**
 * @brief Interface and base class for a parser that parses a response payload
 * into a WpaResponse object, typically a derived class of WpaResponse.
 */
struct WpaResponseParser
{
    WpaResponseParser() = delete;

    virtual ~WpaResponseParser() = default;

    /**
     * Prevent copy and move operations. 
     */
    WpaResponseParser(const WpaResponseParser&) = delete;
    WpaResponseParser(WpaResponseParser&&) = delete;
    WpaResponseParser& operator=(const WpaResponseParser&) = delete;
    WpaResponseParser& operator=(WpaResponseParser&&) = delete;

    /**
     * @brief Construct a new WpaResponseParser object.
     *
     * @param command The command associated with the response payload to be parsed
     * @param responsePayload The response payload to parse.
     * @param propertiesToParse The properties to parse from the response payload.
     */
    WpaResponseParser(const WpaCommand* command, std::string_view responsePayload, std::initializer_list<WpaKeyValuePair> propertiesToParse);

    /**
     * @brief Parses the response payload, returning a WpaResponse object if
     * successful.
     *
     * @return std::shared_ptr<WpaResponse>
     */
    virtual std::shared_ptr<WpaResponse>
    Parse();

    /**
     * @brief Get the command associated with the response payload.
     * 
     * @return const WpaCommand* 
     */
    const WpaCommand*
    GetCommand() const noexcept;

    /**
     * @brief Get the response payload.
     * 
     * @return std::string_view 
     */
    std::string_view
    GetResponsePayload() const noexcept;

protected:
    /**
     * @brief Parse the payload. This function is only called if all properties
     * have been resolved and placed into the m_properties map, so this
     * implementation may rely on and make use of that.
     *
     * @return std::shared_ptr<WpaResponse>
     */
    virtual std::shared_ptr<WpaResponse>
    ParsePayload() = 0;

    /**
     * @brief Obtain the map of parsed properties.
     *
     * @return const std::unordered_map<std::string_view, std::string_view>&
     */
    const std::unordered_map<std::string_view, std::string_view>&
    GetProperties() const noexcept;

private:
    /**
     * @brief Attempts to parse the properties specified in the constructor. For
     * each property successfully parsed, an entry will be added to the
     * m_properties map, and the corresponding entry in m_propertiesToParse is
     * removed.
     *
     * @return true If all properties were successully parsed.
     * @return false If not all properties were successully parsed.
     */
    bool
    TryParseProperties();

private:
    const WpaCommand* m_command;
    std::string_view m_responsePayload;
    std::vector<WpaKeyValuePair> m_propertiesToParse;
    std::unordered_map<std::string_view, std::string_view> m_properties{};
};

/**
 * @brief Interface for a factory that creates WpaResponseParser objects.
 */
struct WpaResponseParserFactory
{
    WpaResponseParserFactory() = default;

    virtual ~WpaResponseParserFactory() = default;

    /**
     * Prevent copy and move operations. 
     */
    WpaResponseParserFactory(const WpaResponseParserFactory&) = delete;
    WpaResponseParserFactory(WpaResponseParserFactory&&) = delete;
    WpaResponseParserFactory& operator=(const WpaResponseParserFactory&) = delete;
    WpaResponseParserFactory& operator=(WpaResponseParserFactory&&) = delete;

    /**
     * @brief Create a response parser object.
     *
     * @param command The command to create the parser object for.
     * @param responsePayload The payload the parser should parse.
     * @return std::unique_ptr<WpaResponseParser>
     */
    virtual std::unique_ptr<WpaResponseParser>
    CreateResponseParser(const WpaCommand* command, std::string_view responsePayload) const = 0;
};
} // namespace Wpa

#endif // WPA_RESPONSE_PARSER_HXX
