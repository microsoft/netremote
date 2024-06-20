
#ifndef NET_REMOTE_SERVER_CONFIGURATION_HXX
#define NET_REMOTE_SERVER_CONFIGURATION_HXX

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <microsoft/net/NetworkManager.hxx>
#include <microsoft/net/remote/protocol/NetRemoteProtocol.hxx>
#include <microsoft/net/remote/service/NetRemoteDiscoveryService.hxx>

namespace Microsoft::Net::Remote::Service
{
/**
 * @brief Collects configuration options for the NetRemoteServer class.
 */
struct NetRemoteServerConfiguration
{
    /**
     * @brief Default log verbosity.
     */
    static constexpr auto LogVerbosityDefault = 3;

    /**
     * @brief Create a NetRemoteServerConfiguration object from command-line
     * arguments.
     *
     * @param argc
     * @param argv
     * @param throwOnParseError
     * @return NetRemoteServerConfiguration
     */
    static NetRemoteServerConfiguration
    FromCommandLineArguments(int argc, char* argv[], bool throwOnParseError = false);

    /**
     * @brief Create a NetRemoteServerConfiguration object from command-line
     * arguments.
     *
     * @param args
     * @param throwOnParseError
     * @return NetRemoteServerConfiguration
     */
    static NetRemoteServerConfiguration
    FromCommandLineArguments(std::vector<std::string>& args, bool throwOnParseError = false);

    /**
     * @brief Default address for the server to listen on.
     */
    std::string ServerAddress{ Microsoft::Net::Remote::Protocol::NetRemoteProtocol::AddressDefault };

    /**
     * @brief Run the service in the background.
     */
    bool RunInBackground{ false };

    /**
     * @brief Log verbosity threshold. The default level is 'info' (3) which will print all log messages with verbosity
     * level info, warning, error, and fatal.
     *
     * --------------------
     * | level | severity |
     * | ----- | -------- |
     * |   0   |  fatal   |
     * |   1   |  error   |
     * |   2   |  warning |
     * |   3   |  info    |
     * |   4   |  debug   |
     * |   5+  |  verbose |
     * --------------------
     */
    uint32_t LogVerbosity{ LogVerbosityDefault };

    /**
     * @brief Whether to enable logging to file or not.
     */
    bool EnableFileLogging{ false };

    /**
     * @brief Object to use when performing network operations.
     */
    std::shared_ptr<Microsoft::Net::NetworkManager> NetworkManager{};

    /**
     * @brief Factory to use to create the discovery service.
     */
    std::shared_ptr<INetRemoteDiscoveryServiceFactory> DiscoveryServiceFactory{};
};

} // namespace Microsoft::Net::Remote::Service

#endif // NET_REMOTE_SERVER_CONFIGURATION_HXX
