
#ifndef NET_REMOTE_SERVER_HXX
#define NET_REMOTE_SERVER_HXX

#include <memory>
#include <string_view>
#include <string>

#include <CLI/CLI.hpp>
#include <grpcpp/server.h>
#include <microsoft/net/remote/NetRemoteService.hxx>

namespace Microsoft::Net::Remote
{
/**
 * @brief Represents a remote server. Note that this class is not thread-safe.
 */
struct NetRemoteServer
{
    /**
     * @brief Default address for the server to listen on.
     */
    static constexpr auto ServerAddressDefault = "0.0.0.0:5047";

    /**
     * @brief Construct a new NetRemoteServer object from command-line arguments.
     * 
     * @param argc The number of arguments.
     * @param argv An array of 'argc' arguments.
     */
    NetRemoteServer(int argc, char* argv[]);

    /**
     * @brief Get the GrpcServer object.
     * 
     * @return std::unique_ptr<grpc::Server>& 
     */
    std::unique_ptr<grpc::Server>& GetGrpcServer() noexcept;

    /**
     * @brief Start the server if not already started.
     */
    void Run();

    /**
     * @brief Stop the server if started.
     */
    void Stop();

private:
    /**
     * @brief Create a CLI11 parser object configured with the supported CLI
     * arguments.
     * 
     * @return std::unique_ptr<CLI::App> 
     */
    static std::unique_ptr<CLI::App> CreateCliParser();

private:
    std::unique_ptr<CLI::App> m_cliParser;
    std::string m_serverAddress{ ServerAddressDefault };
    std::unique_ptr<grpc::Server> m_server;
    Service::NetRemoteService m_service{};
};
} // namespace Microsoft::Net::Remote

#endif // NET_REMOTE_SERVER_HXX
