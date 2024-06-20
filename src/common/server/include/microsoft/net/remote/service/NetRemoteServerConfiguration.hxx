
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
     * @brief How verbose log output should be. The default level is 0, which
     * will show all warnings, errors, and fatal messages. A level of 1 will
     * show all info messages, and a level of 2 will show all debug messages,
     * and a level of 3 or above will show all verbose messages.
     */
    uint32_t LogVerbosity{ 0 };

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
