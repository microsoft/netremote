
#include <format>

#include <errno.h>
#include <logging/LogUtils.hxx>
#include <microsoft/net/remote/NetRemoteServer.hxx>
#include <microsoft/net/remote/NetRemoteServerConfiguration.hxx>
#include <microsoft/net/wifi/AccessPointControllerHostapd.hxx>
#include <microsoft/net/wifi/AccessPointDiscoveryAgent.hxx>
#include <microsoft/net/wifi/AccessPointDiscoveryAgentOperationsNetlink.hxx>
#include <microsoft/net/wifi/AccessPointLinux.hxx>
#include <microsoft/net/wifi/AccessPointManager.hxx>
#include <notstd/Utility.hxx>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Formatters/MessageOnlyFormatter.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Init.h>
#include <plog/Log.h>
#include <unistd.h>

using namespace Microsoft::Net::Remote;
using namespace Microsoft::Net::Wifi;

enum class LogInstanceId : int {
    // Default logger is 0 and is omitted from this enumeration.
    Console = 1,
    File = 2,
};

int
main(int argc, char *argv[])
{
    // Create file and console log appenders.
    static plog::ColorConsoleAppender<plog::MessageOnlyFormatter> colorConsoleAppender{};
    static plog::RollingFileAppender<plog::TxtFormatter> rollingFileAppender(logging::GetLogName("server").c_str());

    // Parse command line arguments.
    auto configuration = NetRemoteServerConfiguration::FromCommandLineArguments(argc, argv);
    const auto logSeverity = logging::LogVerbosityToPlogSeverity(configuration.LogVerbosity);

    // Configure logging, appending all loggers to the default instance.
    plog::init<notstd::to_underlying(LogInstanceId::Console)>(logSeverity, &colorConsoleAppender);
    plog::init<notstd::to_underlying(LogInstanceId::File)>(logSeverity, &rollingFileAppender);
    plog::init(logSeverity)
        .addAppender(plog::get<notstd::to_underlying(LogInstanceId::Console)>())
        .addAppender(plog::get<notstd::to_underlying(LogInstanceId::File)>());

    // Create an access point manager and discovery agent.
    {
        auto accessPointControllerFactory = std::make_unique<AccessPointControllerHostapdFactory>();
        auto accessPointFactory = std::make_unique<AccessPointFactoryLinux>(std::move(accessPointControllerFactory));
        configuration.AccessPointManager = AccessPointManager::Create(std::move(accessPointFactory));

        auto &accessPointManager = configuration.AccessPointManager;
        auto accessPointDiscoveryAgentOperationsNetlink = std::make_unique<AccessPointDiscoveryAgentOperationsNetlink>();
        auto accessPointDiscoveryAgent = AccessPointDiscoveryAgent::Create(std::move(accessPointDiscoveryAgentOperationsNetlink));
        accessPointManager->AddDiscoveryAgent(std::move(accessPointDiscoveryAgent));
    }

    // Create the server.
    NetRemoteServer server{ std::move(configuration) };

    // Start the server.
    LOGI << "Netremote server starting";
    server.Run();

    // If running in the background, daemonize the process.
    if (configuration.RunInBackground) {
        constexpr int nochdir = 0; // Change current working directory to /
        constexpr int noclose = 0; // Don't redirect stdin, stdout to /dev/null

        if (daemon(nochdir, noclose) != 0) {
            const int error = errno;
            const auto what = std::format("Failed to daemonize (error={})", error);
            LOGE << what;
            throw std::runtime_error(what);
        }
    }
    // Otherwise wait for the server to exit.
    else {
        server.GetGrpcServer()->Wait();
    }

    LOGI << "Netremote server stopping";

    return 0;
}
