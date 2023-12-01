
#ifndef NET_REMOTE_SERVER_CONFIGURATION_HXX
#define NET_REMOTE_SERVER_CONFIGURATION_HXX

#include <string>
#include <vector>

namespace Microsoft::Net::Remote
{
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
};

} // namespace Microsoft::Net::Remote

#endif // NET_REMOTE_SERVER_CONFIGURATION_HXX
