
#include <format>
#include <iostream>

// #include "include/microsoft/wifi/remote/WifiRemoteService.hxx"
#include <microsoft/wifi/remote/WifiRemoteService.hxx>

using namespace Microsoft::Wifi::Remote::Service;

::grpc::Status WifiRemoteService::GetWifiApControl([[maybe_unused]] ::grpc::ServerContext* context, const ::Microsoft::Wifi::Remote::Request* request, ::Microsoft::Wifi::Remote::Response* response)
{
    std::cout << std::format("Received GetWifiApControl request with id {} and payload {}", request->id(), request->payload()) << '\n';

    response->set_requestid(request->id());
    response->set_payload("Pong");

    return grpc::Status::OK;
}
