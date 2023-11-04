
#include <grpcpp/server_builder.h>
#include <format>
#include <iostream>
#include <microsoft/net/wifi/remote/WifiRemoteService.hxx>

using Microsoft::Net::Wifi::Remote::Service::WifiRemoteService;

int
main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    static constexpr auto ServerAddress = "0.0.0.0:5047"; 

    WifiRemoteService wifiRemoteService{};

    grpc::ServerBuilder builder{};
    builder.AddListeningPort(ServerAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(&wifiRemoteService);

    auto server{builder.BuildAndStart()};
    std::cout << std::format("Started listening on {}", ServerAddress) << '\n';

    server->Wait();

    return 0;
}
