
#ifndef WPA_COMMAND_HXX
#define WPA_COMMAND_HXX

#include <memory>
#include <string_view>
#include <string>

#include <Wpa/WpaResponseParser.hxx>

namespace Wpa
{
/**
 * @brief Object to hold generic command data for a wpa_supplicant or hostapd
 * request.
 */
struct WpaCommand :
    private WpaResponseParserFactory
{
    virtual ~WpaCommand() = default;

    constexpr WpaCommand() = default;
    constexpr WpaCommand(std::string_view data) :
        Data(data)
    {
    }

    std::shared_ptr<WpaResponse>
    ParseResponse(std::string_view responsePayload);

    std::string Data;

private:
    std::unique_ptr<WpaResponseParser> CreateResponseParser(const WpaCommand* command, std::string_view responsePayload) override;
};
} // namespace Wpa

#endif // WPA_COMMAND_HXX
