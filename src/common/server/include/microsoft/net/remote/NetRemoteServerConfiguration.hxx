
#ifndef NET_REMOTE_SERVER_CONFIGURATION_HXX
#define NET_REMOTE_SERVER_CONFIGURATION_HXX

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace Microsoft::Net::Wifi
{
class AccessPointManager;
} // namespace Microsoft::Net::Wifi

namespace Microsoft::Net::Remote
{
/**
 * @brief Collects configuration options for the NetRemoteServer class.
 */
struct NetRemoteServerConfiguration
{
    /**
     * @brief Default address for the server to listen on.
     */
    static constexpr auto ServerAddressDefault = "0.0.0.0:5047";

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
    std::string ServerAddress{ ServerAddressDefault };

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
     * @brief Access point manager instance.
     */
    std::shared_ptr<Microsoft::Net::Wifi::AccessPointManager> AccessPointManager;
};

} // namespace Microsoft::Net::Remote

#endif // NET_REMOTE_SERVER_CONFIGURATION_HXX
