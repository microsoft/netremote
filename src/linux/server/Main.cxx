
#include <cerrno>
#include <condition_variable>
#include <csignal>
#include <format>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <utility>

#include <logging/LogUtils.hxx>
#include <magic_enum.hpp>
#include <microsoft/net/NetworkManager.hxx>
#include <microsoft/net/NetworkOperationsLinux.hxx>
#include <microsoft/net/remote/service/NetRemoteServer.hxx>
#include <microsoft/net/remote/service/NetRemoteServerConfiguration.hxx>
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

using namespace Microsoft::Net;
using namespace Microsoft::Net::Remote;
using namespace Microsoft::Net::Remote::Service;
using namespace Microsoft::Net::Wifi;

namespace
{
/**
 * @brief Flag indicating if termination of the server has been requested.
 */
bool TerminateRequested{ false };

/**
 * @brief Lock to protect the 'TerminateRequested' termination flag.
 */
std::mutex TerminateGate{};

/**
 * @brief Condition variable to signal that the termination flag 'TerminateRequested' has changed.
 */
std::condition_variable TerminateRequstedChanged{};
} // namespace

/**
 * @brief Signal handler for SIGTERM and SIGINT signals. Sets a flag to request termination of the server.
 *
 * @param signal The signal number.
 */
void
OnSignal(int signal)
{
    if (signal != SIGTERM && signal != SIGINT) {
        LOGW << std::format("Ignoring unexpected signal {}", signal);
        return;
    }

    LOGI << std::format("Received signal {} to terminate server", signal);
    {
        std::unique_lock<std::mutex> terminateRequestedLock{ TerminateGate };
        TerminateRequested = true;
    }

    TerminateRequstedChanged.notify_one();
}

int
main(int argc, char *argv[])
{
    std::signal(SIGTERM, OnSignal);
    std::signal(SIGINT, OnSignal);

    // Create file and console log appenders.
    static plog::ColorConsoleAppender<plog::MessageOnlyFormatter> colorConsoleAppender{};
    std::string logFilePath = "/var/log/";
    logFilePath += logging::GetLogName("server");
    static plog::RollingFileAppender<plog::TxtFormatter> rollingFileAppender(logFilePath.c_str());

    // Create the audit log file appender.
    std::string auditLogFilePath = "/var/log/";
    auditLogFilePath += logging::GetLogName("audit");
    static plog::RollingFileAppender<plog::TxtFormatter> auditLogRollingFileAppender(auditLogFilePath.c_str());

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
    plog::init<std::to_underlying(LogInstanceId::Audit)>(logSeverity, &auditLogRollingFileAppender);

    LOGN << std::format("Netremote server starting (log level={})", magic_enum::enum_name(logSeverity));
    AUDITN << std::format("Netremote server starting (log level={})", magic_enum::enum_name(logSeverity));

    // Create an access point manager and discovery agent.
    auto accessPointManager = AccessPointManager::Create();
    auto accessPointControllerFactory = std::make_unique<AccessPointControllerLinuxFactory>();
    auto accessPointFactory = std::make_shared<AccessPointFactoryLinux>(std::move(accessPointControllerFactory));
    auto accessPointDiscoveryAgentOperationsNetlink = std::make_unique<AccessPointDiscoveryAgentOperationsNetlink>(accessPointFactory);
    auto accessPointDiscoveryAgent = AccessPointDiscoveryAgent::Create(std::move(accessPointDiscoveryAgentOperationsNetlink));

    accessPointManager->AddDiscoveryAgent(std::move(accessPointDiscoveryAgent));

    // Create a network manager.
    {
        auto networkOperations = std::make_unique<NetworkOperationsLinux>();
        auto networkManager = std::make_shared<NetworkManager>(std::move(networkOperations), accessPointManager);
        configuration.NetworkManager = networkManager;
    }

    // Configure service discovery to use DNS-SD.
    configuration.DiscoveryServiceFactory = std::make_shared<NetRemoteDiscoveryServiceLinuxDnssdFactory>();

    // Create the server.
    NetRemoteServer server{ configuration };

    // Start the server.
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
        {
            std::unique_lock<std::mutex> terminateRequestedLock{ TerminateGate };
            TerminateRequstedChanged.wait(terminateRequestedLock, [] {
                return TerminateRequested;
            });
        }

        LOGN << "Netremote server stopping";
        auto &grpcServer = server.GetGrpcServer();
        grpcServer->Shutdown();
        grpcServer->Wait();
    }

    LOGN << "Netremote server stopped";

    return 0;
}
