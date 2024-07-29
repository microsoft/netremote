
#include <string>
#include <utility>
#include <vector>

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Error.hpp>
#include <CLI/Formatter.hpp>
#include <microsoft/net/remote/service/NetRemoteServerConfiguration.hxx>

using namespace Microsoft::Net::Remote::Service;

namespace details
{
CLI::App&
ConfigureCliAppOptions(CLI::App& app, NetRemoteServerConfiguration& config)
{
    app.add_flag(
        "-d,--daemon,-b,--background",
        config.RunInBackground,
        "Run the service as a daemon in the background");

    app.add_option(
        "-a,--address",
        config.ServerAddress,
        "The address to listen on for incoming connections");

    app.add_flag(
        "--enable-file-logging",
        config.EnableFileLogging,
        "Enable logging to file (disabled by default)");

    app.add_option(
        "-c,--config",
        config.JsonConfigurationFilePath,
        "The path to the JSON configuration file");

    app.add_flag(
           "-v,--verbosity",
           config.LogVerbosity,
           "The log verbosity level. Supply multiple times to increase verbosity (0=fatal, 1=errors, 2=warnings, 3=info, 4=debug, 5+=verbose)")
        ->default_val(NetRemoteServerConfiguration::LogVerbosityDefault);

    return app;
}

template <typename... Args>
NetRemoteServerConfiguration
ParseCliAppOptions(bool throwOnParseError, Args&&... args)
{
    CLI::App app{};
    NetRemoteServerConfiguration configuration{};
    ConfigureCliAppOptions(app, configuration);

    try {
        app.parse(std::forward<Args>(args)...);
    } catch (const CLI::ParseError& parseError) {
        if (throwOnParseError) {
            throw parseError; // NOLINT(cert-err60-cpp)
        }
    }

    return configuration;
}

} // namespace details

/* static */
NetRemoteServerConfiguration
NetRemoteServerConfiguration::FromCommandLineArguments(int argc, char* argv[], bool throwOnParseError) // NOLINT(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
{
    return details::ParseCliAppOptions(throwOnParseError, argc, argv);
}

/* static */
NetRemoteServerConfiguration
NetRemoteServerConfiguration::FromCommandLineArguments(std::vector<std::string>& args, bool throwOnParseError)
{
    return details::ParseCliAppOptions(throwOnParseError, args);
}
