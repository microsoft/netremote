
#include <format>
#include <iostream>

#include <microsoft/net/remote/NetRemoteService.hxx>

using namespace Microsoft::Net::Remote::Service;

::grpc::Status NetRemoteService::GetWifiApControl([[maybe_unused]] ::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Request* request, ::Microsoft::Net::Remote::Response* response)
{
    std::cout << std::format("Received GetWifiApControl request with id {} and payload {}\n", request->id(), request->payload());

    response->set_requestid(request->id());
    response->set_payload("Pong");

    return grpc::Status::OK;
}

::grpc::Status NetRemoteService::WifiConfigureAccessPoint([[maybe_unused]] ::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiConfigureAccessPointRequest* request, ::Microsoft::Net::Remote::Wifi::WifiConfigureAccessPointResult* response)
{
    std::cout << std::format("Received WifiConfigureAccessPoint request for access point id {} with {} configurations\n", request->accesspointid(), request->configurations_size());

    response->set_accesspointid(request->accesspointid());
    response->set_succeeded(true);

    return grpc::Status::OK;
}
