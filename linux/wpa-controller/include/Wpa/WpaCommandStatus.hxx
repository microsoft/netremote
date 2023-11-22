
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
    constexpr WpaCommandStatus();

private:
    std::unique_ptr<WpaResponseParser> CreateResponseParser(const WpaCommand* command, std::string_view responsePayload) override;
};

struct WpaStatusResponseParser : public WpaResponseParser
{
    WpaStatusResponseParser(const WpaCommand* command, std::string_view responsePayload);

    std::shared_ptr<WpaResponse> ParsePayload() override;
};

} // namespace Wpa

#endif // WPA_COMMAND_STATUS_HXX
