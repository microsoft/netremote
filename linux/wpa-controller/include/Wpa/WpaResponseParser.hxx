
#ifndef WPA_RESPONSE_PARSER_HXX
#define WPA_RESPONSE_PARSER_HXX

#include <initializer_list>
#include <memory>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <utility>
#include <tuple>

#include <Wpa/WpaCommand.hxx>
#include <Wpa/WpaKeyValuePair.hxx>
#include <Wpa/WpaResponse.hxx>

namespace Wpa
{
struct WpaResponseParser
{
    /**
     * @brief Construct a new WpaResponseParser object.
     * 
     * @param command The command associated with the response payload to be parsed
     * @param responsePayload The response payload to parse.
     * @param propertiesToParse The properties to parse from the response payload.
     */
    WpaResponseParser(const WpaCommand& command, std::string_view responsePayload, std::initializer_list<WpaKeyValuePair> propertiesToParse);

    const WpaCommand& Command;
    const std::string_view ResponsePayload;

    /**
     * @brief Parses the response payload, returning a WpaResponse object if
     * successful.
     * 
     * @return std::shared_ptr<WpaResponse>
     */
    virtual std::shared_ptr<WpaResponse> Parse();

protected:
    /**
     * @brief Parse the payload. This function is only called if all properties
     * have been resolved and placed into the m_properties map, so this
     * implementation may rely on and make use of that.
     * 
     * @return std::shared_ptr<WpaResponse> 
     */
    virtual std::shared_ptr<WpaResponse> ParsePayload() = 0;

    /**
     * @brief Obtain the map of parsed properties.
     * 
     * @return const std::unordered_map<std::string_view, std::string_view>& 
     */
    const std::unordered_map<std::string_view, std::string_view>& GetProperties() noexcept;

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
    bool TryParseProperties();

private:
    std::vector<WpaKeyValuePair> m_propertiesToParse;
    std::unordered_map<std::string_view, std::string_view> m_properties{};
};
} // namespace Wpa

#endif // WPA_RESPONSE_PARSER_HXX