
#ifndef NET_REMOTE_SERVER_HXX
#define NET_REMOTE_SERVER_HXX

#include <memory>
#include <string_view>
#include <string>

#include <grpcpp/server.h>
#include <microsoft/net/remote/NetRemoteService.hxx>

namespace Microsoft::Net::Remote
{
/**
 * @brief Represents a remote server. Note that this class is not thread-safe.
 */
struct NetRemoteServer
{
    virtual ~NetRemoteServer();

    /**
     * @brief Construct a new NetRemoteServer object.
     * 
     * @param serverAddress 
     */
    NetRemoteServer(std::string_view serverAddress);

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
    std::string m_serverAddress;
    std::unique_ptr<grpc::Server> m_server;
    Service::NetRemoteService m_service{};
};
} // namespace Microsoft::Net::Remote

#endif // NET_REMOTE_SERVER_HXX
