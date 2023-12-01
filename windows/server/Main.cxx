
#include <format>
#include <iostream>

#include <microsoft/net/remote/NetRemoteServerConfiguration.hxx>
#include <microsoft/net/remote/NetRemoteServer.hxx>

using namespace Microsoft::Net::Remote;

int main(int argc, char* argv[])
{
    const auto configuration = NetRemoteServerConfiguration::FromCommandLineArguments(argc, argv);

    NetRemoteServer server{ configuration.ServerAddress };
    server.Run();
    server.GetGrpcServer()->Wait();

    return 0;
}
