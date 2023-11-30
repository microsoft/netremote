
#include <format>
#include <iostream>

#include <microsoft/net/remote/NetRemoteServerConfiguration.hxx>
#include <microsoft/net/remote/NetRemoteServer.hxx>
#include <unistd.h>

using namespace Microsoft::Net::Remote;

int
main(int argc, char* argv[])
{
    const auto configuration = NetRemoteServerConfiguration::FromCommandLineArguments(argc, argv);

    NetRemoteServer server{ configuration.ServerAddress };
    server.Run();

    if (configuration.RunInBackground)
    {
        constexpr int nochdir = 0; // Change current working directory to /
        constexpr int noclose = 0; // Don't redirect stdin, stdout to /dev/null

        if (daemon(nochdir, noclose) != 0) {
            const int error = errno;
            const auto what = std::format("failed to daemonize (error={})", error);
            std::cerr << what << std::endl;
            throw std::runtime_error(what);
        }
    }

    return 0;
}
