
#ifndef I_NET_REMOTE_CLI_HANDLER_OPERATIONS_HXX
#define I_NET_REMOTE_CLI_HANDLER_OPERATIONS_HXX

#include <memory>

#include <microsoft/net/remote/NetRemoteServerConnection.hxx>

namespace Microsoft::Net::Remote
{
/**
 * @brief Operations that can be carried out by a NetRemote CLI handler.
 */
struct INetRemoteCliHandlerOperations
{
    virtual ~INetRemoteCliHandlerOperations() = default;

    /**
     * @brief Enumerate available WiFi access points.
     */
    virtual void
    WifiEnumerateAccessPoints() = 0;
};

/**
 * @brief Factory to create instances of INetRemoteCliHandlerOperations.
 */
struct INetRemoteCliHandlerOperationsFactory
{
    virtual ~INetRemoteCliHandlerOperationsFactory() = default;

    /**
     * @brief Create a new INetRemoteCliHandlerOperationsFactory instance with the specified server connection.
     *
     * @param connection The server connection the handler will use to carry out operations.
     * @return std::unique_ptr<INetRemoteCliHandlerOperations>
     */
    virtual std::unique_ptr<INetRemoteCliHandlerOperations>
    Create(std::shared_ptr<NetRemoteServerConnection> connection) = 0;
};
} // namespace Microsoft::Net::Remote

#endif // I_NET_REMOTE_CLI_HANDLER_OPERATIONS_HXX
