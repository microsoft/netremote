
#ifndef NET_REMOTE_SERVER_HXX
#define NET_REMOTE_SERVER_HXX

#include <memory>
#include <string>

#include <grpcpp/server.h>
#include <microsoft/net/NetworkManager.hxx>
#include <microsoft/net/remote/service/NetRemoteDataStreamingService.hxx>
#include <microsoft/net/remote/service/NetRemoteServerConfiguration.hxx>
#include <microsoft/net/remote/service/NetRemoteService.hxx>

namespace Microsoft::Net::Remote::Service
{
/**
 * @brief Represents a remote server. Note that this class is not thread-safe.
 */
struct NetRemoteServer
{
    virtual ~NetRemoteServer();

    NetRemoteServer() = delete;

    /**
     * @brief Construct a new NetRemoteServer object with the specified configuration.
     *
     * @param configuration
     */
    explicit NetRemoteServer(const NetRemoteServerConfiguration& configuration);

    /**
     * Prevent copying and moving of this object.
     */
    NetRemoteServer(const NetRemoteServer&) = delete;

    NetRemoteServer(NetRemoteServer&&) = delete;

    NetRemoteServer&
    operator=(const NetRemoteServer&) = delete;

    NetRemoteServer&
    operator=(NetRemoteServer&&) = delete;

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
    NetRemoteService&
    GetService() noexcept;

    /**
     * @brief Get the NetRemoteDataStreamingService object instance.
     *
     * @return Service::NetRemoteDataStreamingService&
     */
    NetRemoteDataStreamingService&
    GetDataStreamingService() noexcept;

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

protected:
    /**
     * @brief Initialize the discovery service, if its configuration was provided.
     */
    void
    InitializeDiscoveryService();

private:
    std::string m_serverAddress;
    std::shared_ptr<Microsoft::Net::NetworkManager> m_networkManager;
    std::shared_ptr<INetRemoteDiscoveryServiceFactory> m_discoveryServiceFactory;
    std::shared_ptr<NetRemoteDiscoveryService> m_discoveryService;
    std::unique_ptr<grpc::Server> m_server;

    NetRemoteService m_service;
    NetRemoteDataStreamingService m_dataStreamingService;
};
} // namespace Microsoft::Net::Remote::Service

#endif // NET_REMOTE_SERVER_HXX
