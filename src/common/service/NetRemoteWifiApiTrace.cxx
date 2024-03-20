
#include <optional>
#include <source_location>
#include <string>
#include <utility>

#include <magic_enum.hpp>
#include <microsoft/net/remote/protocol/NetRemoteWifi.pb.h>
#include <plog/Severity.h>

#include "NetRemoteApiTrace.hxx"
#include "NetRemoteWifiApiTrace.hxx"

using namespace Microsoft::Net::Remote::Service::Tracing;

using Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus;
using Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatusCode;

NetRemoteWifiApiTrace::NetRemoteWifiApiTrace(std::optional<std::string> accessPointId, const WifiAccessPointOperationStatus* operationStatus, plog::Severity logSeverity, std::source_location location) :
    NetRemoteApiTrace(/* deferEnter= */ true, logSeverity, location),
    m_accessPointId(std::move(accessPointId)),
    m_operationStatus(operationStatus)
{
    if (m_accessPointId.has_value()) {
        AddArgument(ArgNameAccessPointId, m_accessPointId.value());
    }

    Enter();
}

NetRemoteWifiApiTrace::~NetRemoteWifiApiTrace()
{
    if (m_operationStatus == nullptr) {
        return;
    }

    AddReturnValue(ArgNameStatus, std::string(magic_enum::enum_name(m_operationStatus->code())));

    if (m_operationStatus->code() != WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded) {
        AddReturnValue(ArgNameErrorMessage, m_operationStatus->message());
        SetFailed();
    } else {
        SetSucceeded();
    }
}
