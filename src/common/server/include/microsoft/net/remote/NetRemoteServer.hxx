
#ifndef NET_REMOTE_SERVER_HXX
#define NET_REMOTE_SERVER_HXX

#include <memory>
#include <string>

#include <grpcpp/server.h>
#include <microsoft/net/remote/NetRemoteDataStreamingService.hxx>
#include <microsoft/net/remote/NetRemoteServerConfiguration.hxx>
#include <microsoft/net/remote/NetRemoteService.hxx>

namespace Microsoft::Net::Remote
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
    Service::NetRemoteService&
    GetService() noexcept;

    /**
     * @brief Get the NetRemoteDataStreamingService object instance.
     *
     * @return Service::NetRemoteDataStreamingService&
     */
    Service::NetRemoteDataStreamingService&
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
    std::unique_ptr<grpc::Server> m_server;
    Service::NetRemoteService m_service;
    Service::NetRemoteDataStreamingService m_dataStreamingService;
    std::shared_ptr<INetRemoteDiscoveryServiceFactory> m_discoveryServiceFactory;
    std::shared_ptr<INetworkOperations> m_networkOperations;
    std::shared_ptr<NetRemoteDiscoveryService> m_discoveryService;
};
} // namespace Microsoft::Net::Remote

#endif // NET_REMOTE_SERVER_HXX
