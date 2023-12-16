
#include <format>
#include <iostream>

#include <microsoft/net/remote/NetRemoteService.hxx>
#include <plog/Log.h>

using namespace Microsoft::Net::Remote::Service;

::grpc::Status
NetRemoteService::WifiConfigureAccessPoint([[maybe_unused]] ::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiConfigureAccessPointRequest* request, ::Microsoft::Net::Remote::Wifi::WifiConfigureAccessPointResult* response)
{
    LOG_VERBOSE << std::format("Received WifiConfigureAccessPoint request for access point id {} with {} configurations\n", request->accesspointid(), request->configurations_size());

    response->set_accesspointid(request->accesspointid());
    response->set_succeeded(true);

    return grpc::Status::OK;
}

::grpc::Status
NetRemoteService::WifiEnumerateAccessPoints([[maybe_unused]] ::grpc::ServerContext* context, [[maybe_unused]] const ::Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsRequest* request, [[maybe_unused]] ::Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsResult* response)
{
    LOG_VERBOSE << std::format("Received WifiEnumerateAccessPoints request\n");

    return grpc::Status::OK;
}
