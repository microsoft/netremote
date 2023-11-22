
#include <iostream> // TODO: remove me

#include <Wpa/ProtocolHostapd.hxx>
#include <Wpa/ProtocolWpa.hxx>
#include <Wpa/WpaCommandStatus.hxx>
#include <Wpa/WpaResponseStatus.hxx>

using namespace Wpa;

std::unique_ptr<WpaResponseParser> WpaCommandStatus::CreateResponseParser(const WpaCommand* command, std::string_view responsePayload) const
{
    std::cout << "WpaCommandStatus::CreateResponseParser() called\n";
    return std::make_unique<WpaStatusResponseParser>(command, responsePayload);
}

WpaStatusResponseParser::WpaStatusResponseParser(const WpaCommand* command, std::string_view responsePayload) :
    WpaResponseParser(command, responsePayload, {
        { ProtocolHostapd::ResponseStatusPropertyKeyState, WpaValuePresence::Required },
    })
{
    std::cout << "WpaStatusResponseParser::WpaStatusResponseParser() called\n";
}

std::shared_ptr<WpaResponse> WpaStatusResponseParser::ParsePayload()
{
    std::cout << "WpaStatusResponseParser::ParsePayload() called\n";

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
