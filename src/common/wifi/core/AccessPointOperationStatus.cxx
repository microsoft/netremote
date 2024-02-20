
#include <microsoft/net/wifi/AccessPointOperationStatus.hxx>

using namespace Microsoft::Net::Wifi;

/* static */
AccessPointOperationStatus
AccessPointOperationStatus::MakeSucceeded() noexcept
{
    return AccessPointOperationStatus{ AccessPointOperationStatusCode::Succeeded };
}

AccessPointOperationStatus::operator bool() const noexcept
{
    return (Code == AccessPointOperationStatusCode::Succeeded);
}
