
#ifndef NET_REMOTE_CLI_HANDLER_OPERATIONS_HXX
#define NET_REMOTE_CLI_HANDLER_OPERATIONS_HXX

#include <memory>

#include <microsoft/net/remote/INetRemoteCliHandlerOperations.hxx>
#include <microsoft/net/remote/NetRemoteServerConnection.hxx>

namespace Microsoft::Net::Remote
{
/**
 * @brief Base implementation for handling CLI operations. This uses a remote server connection to carry out the
 * operations.
 */
struct NetRemoteCliHandlerOperations :
    public INetRemoteCliHandlerOperations
{
    ~NetRemoteCliHandlerOperations() override = default;

    NetRemoteCliHandlerOperations() = delete;

    /**
     * Prevent copying and moving of this object. 
     */
    NetRemoteCliHandlerOperations(const NetRemoteCliHandlerOperations&) = delete;
    NetRemoteCliHandlerOperations(NetRemoteCliHandlerOperations&&) = delete;
    NetRemoteCliHandlerOperations& operator=(const NetRemoteCliHandlerOperations&) = delete;
    NetRemoteCliHandlerOperations& operator=(NetRemoteCliHandlerOperations&&) = delete;

    /**
     * @brief Construct a new NetRemoteCliHandlerOperations object with the specified server connection.
     *
     * @param connection The server connection to use to carry out operations.
     */
    explicit NetRemoteCliHandlerOperations(std::shared_ptr<NetRemoteServerConnection> connection);

    /**
     * @brief Enumerate available WiFi access points.
     */
    void
    WifiEnumerateAccessPoints() override;

private:
    std::shared_ptr<NetRemoteServerConnection> m_connection;
};

struct NetRemoteCliHandlerOperationsFactory :
    public INetRemoteCliHandlerOperationsFactory
{
    NetRemoteCliHandlerOperationsFactory() = default;

    ~NetRemoteCliHandlerOperationsFactory() override = default;

    /**
     * Prevent copying and moving of this object. 
     */
    NetRemoteCliHandlerOperationsFactory(const NetRemoteCliHandlerOperationsFactory&) = delete;
    NetRemoteCliHandlerOperationsFactory(NetRemoteCliHandlerOperationsFactory&&) = delete;
    NetRemoteCliHandlerOperationsFactory& operator=(const NetRemoteCliHandlerOperationsFactory&) = delete;
    NetRemoteCliHandlerOperationsFactory& operator=(NetRemoteCliHandlerOperationsFactory&&) = delete;

    /**
     * @brief Create a new INetRemoteCliHandlerOperationsFactory instance with the specified server connection.
     *
     * @param connection The server connection the handler will use to carry out operations.
     * @return std::unique_ptr<INetRemoteCliHandlerOperations>
     */
    std::unique_ptr<INetRemoteCliHandlerOperations>
    Create(std::shared_ptr<NetRemoteServerConnection> connection) override;
};
} // namespace Microsoft::Net::Remote

#endif // NET_REMOTE_CLI_HANDLER_OPERATIONS_HXX
