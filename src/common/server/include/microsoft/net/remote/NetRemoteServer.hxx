
#ifndef NET_REMOTE_SERVER_HXX
#define NET_REMOTE_SERVER_HXX

#include <memory>
#include <string>
#include <string_view>

#include <grpcpp/server.h>
#include <microsoft/net/remote/NetRemoteServerConfiguration.hxx>
#include <microsoft/net/remote/NetRemoteService.hxx>
#include <microsoft/net/wifi/AccessPointManager.hxx>

namespace Microsoft::Net::Remote
{
/**
 * @brief Represents a remote server. Note that this class is not thread-safe.
 */
struct NetRemoteServer
{
    virtual ~NetRemoteServer();

    /**
     * @brief Construct a new NetRemoteServer object with the specified configuration.
     *
     * @param configuration
     */
    NetRemoteServer(NetRemoteServerConfiguration configuration);

    /**
     * @brief Get the GrpcServer object.
     *
     * @return std::unique_ptr<grpc::Server>&
     */
    std::unique_ptr<grpc::Server>&
    GetGrpcServer() noexcept;

    /**
     * @brief Get the NetRemoteService object instance.
     *
     * @return Service::NetRemoteService&
     */
    Service::NetRemoteService&
    GetService() noexcept;

    /**
     * @brief Start the server if not already started.
     */
    void
    Run();

    /**
     * @brief Stop the server if started.
     */
    void
    Stop();

private:
    std::string m_serverAddress;
    std::unique_ptr<grpc::Server> m_server;
    Service::NetRemoteService m_service;
};
} // namespace Microsoft::Net::Remote

#endif // NET_REMOTE_SERVER_HXX
