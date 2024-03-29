
#include <memory>
#include <string_view>

#include <Wpa/WpaCommand.hxx>
#include <Wpa/WpaResponse.hxx>
#include <Wpa/WpaResponseParser.hxx>

using namespace Wpa;

void
WpaCommand::SetPayload(std::string_view payload)
{
    m_payload = payload;
}

std::string_view
WpaCommand::GetPayload() const noexcept
{
    return m_payload;
}

std::shared_ptr<WpaResponse>
WpaCommand::ParseResponse(std::string_view responsePayload) const
{
    std::shared_ptr<WpaResponse> response;

    auto parser = CreateResponseParser(this, responsePayload);
    if (parser != nullptr) {
        response = parser->Parse();
    } else {
        response = std::make_shared<WpaResponse>(responsePayload);
    }

    return response;
}

std::unique_ptr<WpaResponseParser>
WpaCommand::CreateResponseParser([[maybe_unused]] const WpaCommand* command, [[maybe_unused]] std::string_view responsePayload) const
{
    // Basic commands don't need a response parser since they only provide OK/FAIL results.
    return nullptr;
}
