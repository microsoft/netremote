
#include <format>

#include <errno.h>
#include <logging/LogUtils.hxx>
#include <microsoft/net/remote/NetRemoteServer.hxx>
#include <microsoft/net/remote/NetRemoteServerConfiguration.hxx>
#include <notstd/Utility.hxx>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Appenders/RollingFileAppender.h>
#include <plog/Formatters/MessageOnlyFormatter.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Init.h>
#include <plog/Log.h>
#include <unistd.h>

using namespace Microsoft::Net::Remote;

enum class LogInstanceId : int {
    // Default logger is 0 and is omitted from this enumeration.
    Console = 1,
    File = 2,
};

int
main(int argc, char* argv[])
{
    // Create file and console log appenders.
    static plog::ColorConsoleAppender<plog::MessageOnlyFormatter> colorConsoleAppender{};
    static plog::RollingFileAppender<plog::TxtFormatter> rollingFileAppender(logging::GetLogName("server").c_str());

    // Parse command line arguments.
    const auto configuration = NetRemoteServerConfiguration::FromCommandLineArguments(argc, argv);
    const auto logSeverity = logging::LogVerbosityToPlogSeverity(configuration.LogVerbosity);

    // Configure logging, appending all loggers to the default instance.
    plog::init<notstd::to_underlying(LogInstanceId::Console)>(logSeverity, &colorConsoleAppender);
    plog::init<notstd::to_underlying(LogInstanceId::File)>(logSeverity, &rollingFileAppender);
    plog::init(logSeverity)
        .addAppender(plog::get<notstd::to_underlying(LogInstanceId::Console)>())
        .addAppender(plog::get<notstd::to_underlying(LogInstanceId::File)>());

    // Start the server.
    NetRemoteServer server{ configuration.ServerAddress };
    server.Run();

    // If running in the background, daemonize the process.
    if (configuration.RunInBackground) {
        constexpr int nochdir = 0; // Change current working directory to /
        constexpr int noclose = 0; // Don't redirect stdin, stdout to /dev/null

        if (daemon(nochdir, noclose) != 0) {
            const int error = errno;
            const auto what = std::format("failed to daemonize (error={})", error);
            LOG_ERROR << what;
            throw std::runtime_error(what);
        }
    }
    // Otherwise wait for the server to exit.
    else {
        server.GetGrpcServer()->Wait();
    }

    LOG_INFO << "Server exiting.";

    return 0;
}
