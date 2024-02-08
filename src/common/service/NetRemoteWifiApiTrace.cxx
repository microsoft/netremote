
#include <optional>
#include <string>
#include <source_location>
#include <utility>

#include "NetRemoteApiTrace.hxx"
#include "NetRemoteWifiApiTrace.hxx"
#include <magic_enum.hpp>
#include <microsoft/net/remote/protocol/NetRemoteWifi.pb.h>

using namespace Microsoft::Net::Remote::Service::Tracing;

using Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus;
using Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatusCode;

NetRemoteWifiApiTrace::NetRemoteWifiApiTrace(std::optional<std::string> accessPointId, const WifiAccessPointOperationStatus* operationStatus, std::source_location location) :
    NetRemoteApiTrace(/* deferEnter= */ true, location),
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
    if (m_operationStatus != nullptr) {
        AddReturnValue(ArgNameStatus, std::string(magic_enum::enum_name(m_operationStatus->code())));
        if (m_operationStatus->code() != WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded) {
            AddReturnValue(ArgNameErrorMessage, m_operationStatus->message());
            SetFailed();
        } else {
            SetSucceeded();
        }
    }
}
