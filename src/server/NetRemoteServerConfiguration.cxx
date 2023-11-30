
#include <utility>

#include <CLI/CLI.hpp>
#include <microsoft/net/remote/NetRemoteServerConfiguration.hxx>

using namespace Microsoft::Net::Remote;

namespace details
{
CLI::App&
ConfigureCliAppOptions(CLI::App& app, NetRemoteServerConfiguration& config)
{
    app.add_flag(
        "-d",
        config.RunInBackground,
        "Run the service as a daemon in the background");

    app.add_option(
        "-a,--address",
        config.ServerAddress,
        "The address to listen on for incoming connections.");

    return app;
}

template <typename... Args>
NetRemoteServerConfiguration
ParseCliAppOptions(bool throwOnParseError, Args&&... args)
{
    CLI::App app{};
    NetRemoteServerConfiguration configuration{};
    ConfigureCliAppOptions(app, configuration);

    try
    {
        app.parse(std::forward<Args>(args)...);
    }
    catch (const CLI::ParseError& parseError)
    {
        if (throwOnParseError)
        {
            throw parseError;
        }
    }

    return configuration;
}

} // namespace details

/* static */
NetRemoteServerConfiguration
NetRemoteServerConfiguration::FromCommandLineArguments(int argc, char* argv[], bool throwOnParseError)
{
    return details::ParseCliAppOptions(throwOnParseError, argc, argv);
}

/* static */
NetRemoteServerConfiguration
NetRemoteServerConfiguration::FromCommandLineArguments(std::vector<std::string>& args, bool throwOnParseError)
{
    return details::ParseCliAppOptions(throwOnParseError, args);
}
