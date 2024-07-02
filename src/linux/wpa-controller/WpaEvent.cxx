
#include <charconv>
#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

#include <Wpa/ProtocolWpa.hxx>
#include <Wpa/WpaCore.hxx>
#include <Wpa/WpaEvent.hxx>
#include <magic_enum.hpp>

using namespace Wpa;

/* static */
std::optional<WpaEvent>
WpaEvent::Parse(std::string_view eventPayload)
{
    WpaEvent event;

    // Find the log level delimeters.
    const std::size_t logLevelStart = eventPayload.find(ProtocolWpa::EventLogLevelDelimeterStart);
    const std::size_t logLevelEnd = (logLevelStart != std::string::npos) ? eventPayload.find(ProtocolWpa::EventLogLevelDelimeterEnd, logLevelStart) : std::string::npos;
    if (logLevelEnd == std::string::npos) {
        return std::nullopt;
    }

    // Parse the log level.
    std::underlying_type_t<WpaLogLevel> logLevelValue{};
    const auto logLevel = eventPayload.substr(logLevelStart + 1, logLevelEnd - logLevelStart - 1);
    const auto logLevelConversionResult = std::from_chars(std::data(logLevel), std::data(logLevel) + std::size(logLevel), logLevelValue);
    if (logLevelConversionResult.ec != std::errc{}) {
        return std::nullopt;
    }

    auto logLevelEnum = magic_enum::enum_cast<WpaLogLevel>(logLevelValue);
    if (logLevelEnum.has_value()) {
        event.LogLevel = logLevelEnum.value();
    }

    // Find the interface name prefix, if present (it's only present with messages from global control sockets).
    const std::size_t interfaceStart = eventPayload.find(ProtocolWpa::EventInterfaceNamePrefix);
    const std::size_t interfaceEnd = (interfaceStart != std::string::npos) ? eventPayload.find(' ', interfaceStart) : std::string::npos;
    if (interfaceEnd != std::string::npos) {
        event.Interface = eventPayload.substr(interfaceStart, interfaceEnd - interfaceStart);
    }

    // Find the payload.
    const std::size_t payloadStart = (interfaceEnd != std::string::npos) ? interfaceEnd + 1 : logLevelEnd + 1;
    event.Payload = eventPayload.substr(payloadStart);

    return event;
}

std::string
WpaEvent::ToString() const
{
    return std::format("[{}] {}{}{}", magic_enum::enum_name(LogLevel), magic_enum::enum_name(Source), Interface.value_or(""), Payload);
}
