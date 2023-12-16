
#ifndef WPA_COMMAND_STATUS_HXX
#define WPA_COMMAND_STATUS_HXX

#include <memory>
#include <string_view>

#include <Wpa/WpaCommand.hxx>
#include <Wpa/WpaResponseParser.hxx>

namespace Wpa
{
/**
 * @brief Representation of the "STATUS" command.
 */
struct WpaCommandStatus :
    public WpaCommand
{
    /**
     * @brief Construct a new WpaCommandStatus object.
     */
    constexpr WpaCommandStatus() :
        WpaCommand(ProtocolWpa::CommandPayloadStatus)
    {
    }

private:
    std::unique_ptr<WpaResponseParser>
    CreateResponseParser(const WpaCommand* command, std::string_view responsePayload) const override;
};

/**
 * @brief Parser for the "STATUS" command response.
 */
struct WpaStatusResponseParser : public WpaResponseParser
{
    /**
     * @brief Construct a new WpaStatusResponseParser object.
     *
     * @param command The command associated with the response.
     * @param responsePayload The response payload to parse.
     */
    WpaStatusResponseParser(const WpaCommand* command, std::string_view responsePayload);

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

#endif // WPA_COMMAND_STATUS_HXX
