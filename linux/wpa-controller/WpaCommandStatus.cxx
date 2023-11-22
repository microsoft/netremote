
#include <Wpa/ProtocolHostapd.hxx>
#include <Wpa/ProtocolWpa.hxx>
#include <Wpa/WpaCommandStatus.hxx>
#include <Wpa/WpaResponseStatus.hxx>

using namespace Wpa;

constexpr WpaCommandStatus::WpaCommandStatus() :
    WpaCommand(ProtocolWpa::CommandPayloadStatus)
{
}

std::unique_ptr<WpaResponseParser> WpaCommandStatus::CreateResponseParser(const WpaCommand* command, std::string_view responsePayload) const
{
    return std::make_unique<WpaStatusResponseParser>(command, responsePayload);
}

WpaStatusResponseParser::WpaStatusResponseParser(const WpaCommand* command, std::string_view responsePayload) :
    WpaResponseParser(command, responsePayload, {
        { ProtocolHostapd::ResponseStatusPropertyKeyState, WpaValuePresence::Required },
    })
{
}

std::shared_ptr<WpaResponse> WpaStatusResponseParser::ParsePayload()
{
    const auto properties = GetProperties();
    const auto response = std::make_shared<WpaResponseStatus>();

    for (const auto& [key, value] : properties)
    {
        if (key == ProtocolHostapd::ResponseStatusPropertyKeyState)
        {
            response->Status.State = HostapdInterfaceStateFromString(value);
        }
    }

    return response;
}
