
#ifndef WPA_COMMAND_HXX
#define WPA_COMMAND_HXX

#include <memory>
#include <string>
#include <string_view>

#include <Wpa/WpaResponse.hxx>
#include <Wpa/WpaResponseParser.hxx>

namespace Wpa
{
/**
 * @brief Object to hold generic command payload for a wpa_supplicant or hostapd
 * request.
 */
struct WpaCommand :
    private WpaResponseParserFactory
{
    ~WpaCommand() override = default;

    constexpr WpaCommand() = default;
    constexpr explicit WpaCommand(std::string_view payload) :
        Payload(payload)
    {
    }

    /**
     * Prevent copying and moving of this object. 
     */
    WpaCommand(const WpaCommand&) = delete;
    WpaCommand& operator=(const WpaCommand&) = delete;
    WpaCommand(WpaCommand&&) = delete;
    WpaCommand& operator=(WpaCommand&&) = delete;

    /**
     * @brief Set the payload of the command.
     *
     * @param payload The payload to assign.
     */
    void
    SetPayload(std::string_view payload);

    /**
     * @brief Parse the response payload into a WpaResponse object.
     *
     * @param responsePayload The response payload to parse.
     * @return std::shared_ptr<WpaResponse>
     */
    std::shared_ptr<WpaResponse>
    ParseResponse(std::string_view responsePayload) const;

    std::string Payload;

private:
    /**
     * @brief Create a response parser object for the given command.
     *
     * @param command The command to create a response parser for.
     * @param responsePayload The response payload to parse.
     * @return std::unique_ptr<WpaResponseParser>
     */
    std::unique_ptr<WpaResponseParser>
    CreateResponseParser(const WpaCommand* command, std::string_view responsePayload) const override;
};
} // namespace Wpa

#endif // WPA_COMMAND_HXX
