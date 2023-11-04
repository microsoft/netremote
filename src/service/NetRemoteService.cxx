
#include <format>
#include <iostream>

#include <microsoft/net/remote/NetRemoteService.hxx>

using namespace Microsoft::Net::Remote::Service;

::grpc::Status NetRemoteService::GetWifiApControl([[maybe_unused]] ::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Request* request, ::Microsoft::Net::Remote::Response* response)
{
    std::cout << std::format("Received GetWifiApControl request with id {} and payload {}", request->id(), request->payload()) << '\n';

    response->set_requestid(request->id());
    response->set_payload("Pong");

    return grpc::Status::OK;
}
