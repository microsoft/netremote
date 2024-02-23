
#include <microsoft/net/wifi/AccessPointOperationStatus.hxx>

using namespace Microsoft::Net::Wifi;

/* static */
AccessPointOperationStatus
AccessPointOperationStatus::MakeSucceeded() noexcept
{
    return AccessPointOperationStatus{ AccessPointOperationStatusCode::Succeeded };
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

AccessPointOperationStatus::operator bool() const noexcept
{
    return Succeeded();
    return (Code == AccessPointOperationStatusCode::Succeeded);
}
