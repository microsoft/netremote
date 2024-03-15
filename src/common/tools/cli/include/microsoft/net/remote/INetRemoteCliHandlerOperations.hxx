
#ifndef I_NET_REMOTE_CLI_HANDLER_OPERATIONS_HXX
#define I_NET_REMOTE_CLI_HANDLER_OPERATIONS_HXX

#include <memory>
#include <string_view>

#include <microsoft/net/remote/NetRemoteServerConnection.hxx>

namespace Microsoft::Net::Remote
{
/**
 * @brief Operations that can be carried out by a NetRemote CLI handler.
 */
struct INetRemoteCliHandlerOperations
{
    INetRemoteCliHandlerOperations() = default;

    virtual ~INetRemoteCliHandlerOperations() = default;

    /**
     * Prevent copying and moving of INetRemoteCliHandlerOperations objects.
     */
    INetRemoteCliHandlerOperations(const INetRemoteCliHandlerOperations&) = delete;

    INetRemoteCliHandlerOperations(INetRemoteCliHandlerOperations&&) = delete;

    INetRemoteCliHandlerOperations&
    operator=(const INetRemoteCliHandlerOperations&) = delete;

    INetRemoteCliHandlerOperations&
    operator=(INetRemoteCliHandlerOperations&&) = delete;

    /**
     * @brief Enumerate available WiFi access points.
     */
    virtual void
    WifiEnumerateAccessPoints() = 0;

    /**
     * @brief Enable the specified WiFi access point.
     *
     * @param accessPointId The identifier of the access point to enable.
     */
    virtual void
    WifiAccessPointEnable(std::string_view accessPointId) = 0;

    /**
     * @brief Disable the specified WiFi access point.
     *
     * @param accessPointId The identifier of the access point to disable.
     */
    virtual void
    WifiAccessPointDisable(std::string_view accessPointId) = 0;
};

/**
 * @brief Factory to create instances of INetRemoteCliHandlerOperations.
 */
struct INetRemoteCliHandlerOperationsFactory
{
    INetRemoteCliHandlerOperationsFactory() = default;

    virtual ~INetRemoteCliHandlerOperationsFactory() = default;

    /**
     * Prevent copying and moving of INetRemoteCliHandlerOperationsFactory objects.
     */
    INetRemoteCliHandlerOperationsFactory(const INetRemoteCliHandlerOperationsFactory&) = delete;

    INetRemoteCliHandlerOperationsFactory(INetRemoteCliHandlerOperationsFactory&&) = delete;

    INetRemoteCliHandlerOperationsFactory&
    operator=(const INetRemoteCliHandlerOperationsFactory&) = delete;

    INetRemoteCliHandlerOperationsFactory&
    operator=(INetRemoteCliHandlerOperationsFactory&&) = delete;

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
