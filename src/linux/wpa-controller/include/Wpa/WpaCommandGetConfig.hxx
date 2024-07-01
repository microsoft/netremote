
#ifndef WPA_COMMAND_GET_CONFIG_HXX
#define WPA_COMMAND_GET_CONFIG_HXX

#include <memory>
#include <string_view>

#include <Wpa/WpaCommand.hxx>
#include <Wpa/WpaResponseParser.hxx>

namespace Wpa
{
/**
 * @brief Representation of the "GET_CONFIG" command.
 */
struct WpaCommandGetConfig :
    public WpaCommand
{
    /**
     * @brief Construct a new WpaCommandGetConfig object.
     */
    constexpr WpaCommandGetConfig() :
        WpaCommand(ProtocolWpa::CommandPayloadGetConfig)
    {
    }

private:
    std::unique_ptr<WpaResponseParser>
    CreateResponseParser(const WpaCommand* command, std::string_view responsePayload) const override;
};

/**
 * @brief Parser for the "GET_CONFIG" command response.
 */
struct WpaGetConfigResponseParser :
    public WpaResponseParser
{
    /**
     * @brief Construct a new WpaGetConfigResponseParser object.
     *
     * @param command The command associated with the response.
     * @param responsePayload The response payload to parse.
     */
    WpaGetConfigResponseParser(const WpaCommand* command, std::string_view responsePayload);

    /**
     * @brief Parses the response payload, returning a WpaResponseStatus object
     * if successful.
     *
     * @return std::shared_ptr<WpaResponse>
     */
    std::shared_ptr<WpaResponse>
    ParsePayload() override;
};

} // namespace Wpa

#endif // WPA_COMMAND_GET_CONFIG_HXX
