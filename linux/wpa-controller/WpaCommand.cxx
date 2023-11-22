
#include <Wpa/WpaCommand.hxx>

using namespace Wpa;

std::shared_ptr<WpaResponse>
WpaCommand::ParseResponse(std::string_view responsePayload)
{
    std::shared_ptr<WpaResponse> response;

    auto parser = CreateResponseParser(this, responsePayload);
    if (parser != nullptr)
    {
        response = parser->Parse();
    }
    else
    {
        response = std::make_shared<WpaResponse>(responsePayload);
    }

    return response;
}

std::unique_ptr<WpaResponseParser> WpaCommand::CreateResponseParser([[maybe_unused]] const WpaCommand* command, [[maybe_unused]] std::string_view responsePayload)
{
    // Basic commands don't need a response parser since they only provide OK/FAIL results.
    return nullptr;
}
