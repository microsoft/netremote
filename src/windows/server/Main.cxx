
#include <format>
#include <iostream>
#include <memory>

#include <microsoft/net/NetworkManager.hxx>
#include <microsoft/net/remote/service/NetRemoteServer.hxx>
#include <microsoft/net/remote/service/NetRemoteServerConfiguration.hxx>
#include <microsoft/net/wifi/AccessPoint.hxx>
#include <microsoft/net/wifi/AccessPointManager.hxx>

using namespace Microsoft::Net;
using namespace Microsoft::Net::Remote;
using namespace Microsoft::Net::Remote::Service;
using namespace Microsoft::Net::Wifi;

int
main(int argc, char* argv[])
{
    auto configuration = NetRemoteServerConfiguration::FromCommandLineArguments(argc, argv);
    {
        auto accessPointManager = AccessPointManager::Create();
        auto networkManager = std::make_shared<NetworkManager>(nullptr, accessPointManager);
        configuration.NetworkManager = networkManager;
    }

    NetRemoteServer server{ std::move(configuration) };
    server.Run();
    server.GetGrpcServer()->Wait();

    return 0;
}
