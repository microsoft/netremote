
#include <format>
#include <iostream>
#include <memory>

#include <microsoft/net/remote/NetRemoteServer.hxx>
#include <microsoft/net/remote/NetRemoteServerConfiguration.hxx>
#include <microsoft/net/wifi/AccessPoint.hxx>
#include <microsoft/net/wifi/AccessPointManager.hxx>

using namespace Microsoft::Net::Remote;
using namespace Microsoft::Net::Wifi;

int
main(int argc, char* argv[])
{
    auto configuration = NetRemoteServerConfiguration::FromCommandLineArguments(argc, argv);
    {
        auto accessPointManager = AccessPointManager::Create();
        configuration.AccessPointManager = accessPointManager;
    }

    NetRemoteServer server{ std::move(configuration) };
    server.Run();
    server.GetGrpcServer()->Wait();

    return 0;
}
