
#include <grpcpp/server_builder.h>
#include <format>
#include <iostream>
#include <microsoft/net/remote/NetRemoteService.hxx>

using Microsoft::Net::Remote::Service::NetRemoteService;

int
main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    static constexpr auto ServerAddress = "0.0.0.0:5047"; 

    NetRemoteService NetRemoteService{};

    grpc::ServerBuilder builder{};
    builder.AddListeningPort(ServerAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(&NetRemoteService);

    auto server{builder.BuildAndStart()};
    std::cout << std::format("Started listening on {}", ServerAddress) << '\n';

    server->Wait();

    return 0;
}
