
#include <microsoft/net/wifi/AccessPointOperationStatusLogOnExit.hxx>
#include <plog/Log.h>
#include <plog/Severity.h>

using namespace Microsoft::Net::Wifi;

AccessPointOperationStatusLogOnExit::AccessPointOperationStatusLogOnExit(AccessPointOperationStatus* operationStatus, plog::Severity severityOnError, plog::Severity severityOnSuccess) noexcept :
    OperationStatus(operationStatus),
    SeverityOnError(severityOnError),
    SeverityOnSuccess(severityOnSuccess)
{
}

AccessPointOperationStatusLogOnExit::~AccessPointOperationStatusLogOnExit()
{
    if (OperationStatus != nullptr) {
        LOG(OperationStatus->Succeeded() ? SeverityOnSuccess : SeverityOnError) << OperationStatus->ToString();
    }
}

void
AccessPointOperationStatusLogOnExit::Reset() noexcept
{
    OperationStatus = nullptr;
}
