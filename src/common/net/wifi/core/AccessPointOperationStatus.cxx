
#include <format>
#include <optional>
#include <source_location>
#include <string>
#include <string_view>

#include <magic_enum.hpp>
#include <microsoft/net/wifi/AccessPointOperationStatus.hxx>

using namespace Microsoft::Net::Wifi;

/* static */
AccessPointOperationStatus
AccessPointOperationStatus::MakeSucceeded(std::string_view accessPointId, std::string_view operationName, std::string_view details, std::source_location sourceLocation) noexcept
{
    return AccessPointOperationStatus{
        accessPointId,
        operationName,
        AccessPointOperationStatusCode::Succeeded,
        details,
        sourceLocation
    };
}

/* static */
AccessPointOperationStatus
AccessPointOperationStatus::InvalidAccessPoint(std::string_view operationName, std::string_view details, std::source_location sourceLocation) noexcept
{
    return AccessPointOperationStatus{
        {},
        operationName,
        AccessPointOperationStatusCode::AccessPointInvalid,
        details,
        sourceLocation
    };
}

bool
AccessPointOperationStatus::Succeeded() const noexcept
{
    return (Code == AccessPointOperationStatusCode::Succeeded);
}

bool
AccessPointOperationStatus::Failed() const noexcept
{
    return !Succeeded();
}

std::string
AccessPointOperationStatus::ToString() const
{
    static constexpr auto Format{ "AP '{}' operation {} {} {} {}" };

    std::optional<std::string> caller{}; // NOLINT(misc-const-correctness)
    const auto result = std::format("{}", Succeeded() ? "succeeded" : std::format("failed with code '{}'", magic_enum::enum_name(Code)));
    const auto details = std::format("{}", std::empty(Details) ? "" : std::format("- {}", Details));
#ifdef DEBUG
    caller = std::format("[{}({}:{}) {}]", SourceLocation.file_name(), SourceLocation.line(), SourceLocation.column(), SourceLocation.function_name());
#endif

    return std::format(Format, AccessPointId, OperationName, result, details, caller.value_or(""));
}

AccessPointOperationStatus::operator bool() const noexcept
{
    return Succeeded();
}
