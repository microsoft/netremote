
#include <format>
#include <iostream>

#include <microsoft/net/remote/NetRemoteServer.hxx>

using namespace Microsoft::Net::Remote;

int
main(int argc, char* argv[])
{
    NetRemoteServer server{ argc, argv };
    server.Run();
    server.GetGrpcServer()->Wait();

    return 0;
}
