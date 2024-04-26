
#include <cerrno>
#include <format>
#include <memory>
#include <stdexcept>
#include <utility>

#include <logging/LogUtils.hxx>
#include <microsoft/net/remote/NetRemoteServer.hxx>
#include <microsoft/net/remote/NetRemoteServerConfiguration.hxx>
#include <microsoft/net/wifi/AccessPointControllerLinux.hxx>
#include <microsoft/net/wifi/AccessPointDiscoveryAgent.hxx>
#include <microsoft/net/wifi/AccessPointDiscoveryAgentOperationsNetlink.hxx>
#include <microsoft/net/wifi/AccessPointLinux.hxx>
#include <microsoft/net/wifi/AccessPointManager.hxx>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Formatters/MessageOnlyFormatter.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Init.h>
#include <plog/Log.h>
#include <plog/Logger.h>
#include <unistd.h>

#include "NetRemoteDiscoveryServiceLinuxDnssd.hxx"
#include "NetworkOperationsLinux.hxx"

using namespace Microsoft::Net;
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
    plog::init<std::to_underlying(LogInstanceId::Console)>(logSeverity, &colorConsoleAppender);
    plog::init(logSeverity).addAppender(plog::get<std::to_underlying(LogInstanceId::Console)>());
    if (configuration.EnableFileLogging) {
        plog::init<std::to_underlying(LogInstanceId::File)>(logSeverity, &rollingFileAppender);
        plog::init(logSeverity).addAppender(plog::get<std::to_underlying(LogInstanceId::File)>());
    }

    // Create an access point manager and discovery agent.
    {
        configuration.AccessPointManager = AccessPointManager::Create();

        auto accessPointControllerFactory = std::make_unique<AccessPointControllerLinuxFactory>();
        auto accessPointFactory = std::make_shared<AccessPointFactoryLinux>(std::move(accessPointControllerFactory));
        auto accessPointDiscoveryAgentOperationsNetlink = std::make_unique<AccessPointDiscoveryAgentOperationsNetlink>(accessPointFactory);
        auto accessPointDiscoveryAgent = AccessPointDiscoveryAgent::Create(std::move(accessPointDiscoveryAgentOperationsNetlink));
        auto &accessPointManager = configuration.AccessPointManager;

        accessPointManager->AddDiscoveryAgent(std::move(accessPointDiscoveryAgent));
    }

    // Configure service discovery to use DNS-SD.
    {
        configuration.NetworkOperations = std::make_unique<NetworkOperationsLinux>();
        configuration.DiscoveryServiceFactory = std::make_unique<NetRemoteDiscoveryServiceLinuxDnssdFactory>();
    }

    // Create the server.
    NetRemoteServer server{ configuration };

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
