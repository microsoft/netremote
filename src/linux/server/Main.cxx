
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
#include <microsoft/net/remote/service/RfAttenuatorFactory.hxx>
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

/**
 * @brief Helper function to get the access point attributes for the specified interface name using a weak reference to
 * an access point manager.
 * 
 * @param accessPointManagerWeak Weak reference to the access point manager.
 * @param interfaceName The interface name of the access point.
 * @return std::optional<Microsoft::Net::Wifi::AccessPointAttributes> 
 */
std::optional<Microsoft::Net::Wifi::AccessPointAttributes>
GetAccessPointAttributes(std::weak_ptr<AccessPointManager> accessPointManagerWeak, const std::string &interfaceName)
{
    // Attempt to resolve the weak reference to a strong one.
    auto accessPointManager{ accessPointManagerWeak.lock() };
    if (!accessPointManager) {
        LOGW << std::format("Access point manager has been destroyed; cannot get access point attributes for interface '{}'", interfaceName);
        return std::nullopt;
    }

    // Get the access point attributes for the specified interface name.
    return accessPointManager->GetAccessPointAttributes(interfaceName);
}
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

std::unique_ptr<IRfAttenuatorController>
CreateSimulatedAttenuator()
{
    // Implementation of CreateSimulatedAttenuator
    RfAttenuatorProperties properties{
        .Channels{ 1, 2, 3, 4 },
        .AttenuationRangeDbmMin = 0,
        .AttenuationRangeDbmMax = 100,
        .AttenuationStepDbmMin = 1,
        .AttenuationStepDbmMax = 5,
        .AttenuationAccuracyDbmMin = 1,
        .AttenuationAccuracyDbmMax = 1,
        .FrequencyBandwidthMHzMin = 0,
        .FrequencyBandwidthMHzMax = 6000,
        .SupportsSweep = false,
        .Identification = "Simulated Attenuator",
    };

    LOGI << "Creating software-based attenuator ... ";

    try {
        auto attenuator = RfAttenuatorFactory::TryCreateBasic("software", std::move(properties));
        LOGI << "succeeded" << std::endl;
        return attenuator;
    } catch (const RfAttenuatorException &e) {
        LOGE << "failed (" << e.what() << ")" << std::endl;
        return nullptr;
    }
}

std::unique_ptr<IRfAttenuatorController>
CreateSocketAttenuator(std::string attenuatorName, std::string ipAddress, uint16_t port)
{
    RfAttenuatorConnectionArgumentsTcp args{
        .IpAddress = std::move(ipAddress),
        .Port = port,
    };

    LOGI << std::format(
                "Creating socket-based attenuator {} @ {}:{}",
                attenuatorName,
                args.IpAddress,
                args.Port)
         << " ... ";

    try {
        auto attenuator = RfAttenuatorFactory::TryCreateWithTcpConnection(attenuatorName, std::move(args));
        LOGI << "succeeded" << std::endl;
        return attenuator;
    } catch (RfAttenuatorException &e) {
        LOGE << "failed (" << e.what() << ")" << std::endl;
        return nullptr;
    } catch (std::exception &e) {
        LOGE << "failed (" << e.what() << ")" << std::endl;
        return nullptr;
    }
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

    LOGN << std::format("Netremote server starting (log level={}, rfAttenuatorType={})", magic_enum::enum_name(logSeverity), magic_enum::enum_name(configuration.RfAttenuatorConfiguration.Type));
    AUDITN << std::format("Netremote server starting (log level={}, rfAttenuatorType={})", magic_enum::enum_name(logSeverity), magic_enum::enum_name(configuration.RfAttenuatorConfiguration.Type));

    // Create an access point manager.
    auto accessPointManager = AccessPointManager::Create(std::move(configuration.AccessPointAttributes));

    // Configure access point discovery agent operations.
    {
        // Create function to look up access point attributes, to be used by discovery agent operations.
        auto getAccessPointAttributes = [accessPointManagerWeak = accessPointManager->weak_from_this()](const std::string &interfaceName) -> std::optional<AccessPointAttributes> {
            return GetAccessPointAttributes(std::move(accessPointManagerWeak), interfaceName);
        };

        auto accessPointControllerFactory = std::make_unique<AccessPointControllerLinuxFactory>();
        auto accessPointFactory = std::make_shared<AccessPointFactoryLinux>(std::move(accessPointControllerFactory));
        auto accessPointDiscoveryAgentOperationsNetlink = std::make_unique<AccessPointDiscoveryAgentOperationsNetlink>(accessPointFactory, std::move(getAccessPointAttributes));
        auto accessPointDiscoveryAgent = AccessPointDiscoveryAgent::Create(std::move(accessPointDiscoveryAgentOperationsNetlink));

        // Add discovery agent to the access point manager. 
        accessPointManager->AddDiscoveryAgent(std::move(accessPointDiscoveryAgent));
    }

    // Create a network manager.
    {
        auto networkOperations = std::make_unique<NetworkOperationsLinux>();
        auto networkManager = std::make_shared<NetworkManager>(std::move(networkOperations), accessPointManager);
        configuration.NetworkManager = networkManager;
    }

    // Configure service discovery to use DNS-SD.
    configuration.DiscoveryServiceFactory = std::make_shared<NetRemoteDiscoveryServiceLinuxDnssdFactory>();

    // Create attenuator controller.
    std::shared_ptr<IRfAttenuatorController> rfAttenuatorController = nullptr;
    if (configuration.RfAttenuatorConfiguration.Type == RfAttenuatorType::Software) {
        rfAttenuatorController = CreateSimulatedAttenuator();
    } else if (configuration.RfAttenuatorConfiguration.Type == RfAttenuatorType::Socket) {
        rfAttenuatorController = CreateSocketAttenuator("AeroflexWeinschle83", configuration.RfAttenuatorConfiguration.Address, configuration.RfAttenuatorConfiguration.Port);
    } else {
        LOGN << "No RF attenuator controller created";
    }

    // Create the server.
    NetRemoteServer server{ configuration, rfAttenuatorController };

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
