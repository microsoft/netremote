
#include <format>
#include <iostream>
#include <iterator>

#include <microsoft/net/remote/NetRemoteService.hxx>
#include <plog/Log.h>

using namespace Microsoft::Net::Remote::Service;

::grpc::Status
NetRemoteService::WifiEnumerateAccessPoints([[maybe_unused]] ::grpc::ServerContext* context, [[maybe_unused]] const ::Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsRequest* request, [[maybe_unused]] ::Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsResult* response)
{
    LOG_VERBOSE << std::format("Received WifiEnumerateAccessPoints request\n");

    return grpc::Status::OK;
}

using Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus;
using Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatusCode;
using Microsoft::Net::Wifi::Dot11AuthenticationAlgorithm;
using Microsoft::Net::Wifi::Dot11CipherAlgorithm;
using Microsoft::Net::Wifi::Dot11PhyType;

::grpc::Status
NetRemoteService::WifiAccessPointEnable([[maybe_unused]] ::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiAccessPointEnableRequest* request, ::Microsoft::Net::Remote::Wifi::WifiAccessPointEnableResult* response)
{
    LOG_VERBOSE << std::format("Received WifiAccessPointEnable request for access point id {}\n", request->accesspointid());

    WifiAccessPointOperationStatus status{};

    // Validate request is well-formed and has all required parameters.
    if (ValidateWifiAccessPointEnableRequest(request, status)) {
        // TODO: Enable the access point.
        status.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);
    }

    response->set_accesspointid(request->accesspointid());
    *response->mutable_status() = std::move(status);

    return grpc::Status::OK;
}

::grpc::Status
NetRemoteService::WifiAccessPointDisable([[maybe_unused]] ::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiAccessPointDisableRequest* request, ::Microsoft::Net::Remote::Wifi::WifiAccessPointDisableResult* response)
{
    LOG_VERBOSE << std::format("Received WifiAccessPointDisable request for access point id {}\n", request->accesspointid());

    WifiAccessPointOperationStatus status{};
    // TODO: Disable the access point.
    status.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);

    response->set_accesspointid(request->accesspointid());
    *response->mutable_status() = std::move(status);

    return grpc::Status::OK;
}

bool
NetRemoteService::ValidateWifiAccessPointEnableRequest(const ::Microsoft::Net::Remote::Wifi::WifiAccessPointEnableRequest* request, ::Microsoft::Net::Remote::Wifi::WifiAccessPointOperationStatus& status)
{
    // Validate required arguments are present. Detailed argument validation is left to the implementation.

    if (!request->has_configuration()) {
        // Configuration isn't required, so exit early.
        return true;
    }

    // Configuration isn't required, but if it's present, it must be valid.
    const auto& configuration = request->configuration();
    if (!configuration.has_ssid()) {
        status.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
        status.set_message("No SSID provided");
        return false;
    }
    if (configuration.phytype() == Dot11PhyType::Dot11PhyTypeUnknown) {
        status.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
        status.set_message("No PHY type provided");
        return false;
    }
    if (configuration.authenticationalgorithm() == Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmUnknown) {
        status.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
        status.set_message("No authentication algorithm provided");
        return false;
    }
    if (configuration.encryptionalgorithm() == Dot11CipherAlgorithm::Dot11CipherAlgorithmUnknown) {
        status.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
        status.set_message("No encryption algorithm provided");
        return false;
    }
    if (std::empty(configuration.bands())) {
        status.set_code(WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
        status.set_message("No radio bands provided");
        return false;
    }

    return true;
}
