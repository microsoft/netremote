
#include <format>
#include <string>
#include <string_view>

#include <magic_enum.hpp>
#include <microsoft/net/wifi/AccessPointOperationStatus.hxx>

using namespace Microsoft::Net::Wifi;

/* static */
AccessPointOperationStatus
AccessPointOperationStatus::MakeSucceeded(std::string_view accessPointId) noexcept
{
    return AccessPointOperationStatus{
        accessPointId,
        AccessPointOperationStatusCode::Succeeded,
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
    static constexpr auto Format{ "AP '{}' operation {} {}" };

    const auto result = std::format("{}", Succeeded() ? "succeeded" : std::format("failed with code '{}'", magic_enum::enum_name(Code)));
    const auto details = std::format("{}", std::empty(Details) ? "" : std::format("({})", Details));

    return std::format(Format, AccessPointId, OperationName, result, details);
}

AccessPointOperationStatus::operator bool() const noexcept
{
    return Succeeded();
    return (Code == AccessPointOperationStatusCode::Succeeded);
}
