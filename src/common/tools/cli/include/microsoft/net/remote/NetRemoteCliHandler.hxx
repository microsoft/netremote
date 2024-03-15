
#ifndef NET_REMOTE_CLI_HANDLER_HXX
#define NET_REMOTE_CLI_HANDLER_HXX

#include <memory>
#include <string_view>

#include <microsoft/net/remote/NetRemoteServerConnection.hxx>

namespace Microsoft::Net::Remote
{
struct INetRemoteCliHandlerOperations;
struct INetRemoteCliHandlerOperationsFactory;
struct NetRemoteCli;

/**
 * @brief Handles NetRemote CLI commands.
 */
struct NetRemoteCliHandler
{
    NetRemoteCliHandler() = default;

    virtual ~NetRemoteCliHandler() = default;

    /**
     * Prevent copying and moving of NetRemoteCliHandler objects.
     */
    NetRemoteCliHandler(const NetRemoteCliHandler&) = delete;

    NetRemoteCliHandler(NetRemoteCliHandler&&) = delete;

    NetRemoteCliHandler&
    operator=(const NetRemoteCliHandler&) = delete;

    NetRemoteCliHandler&
    operator=(NetRemoteCliHandler&&) = delete;

    /**
     * @brief Construct a new NetRemoteCliHandler object.
     *
     * @param operationsFactory The factory to use to create the operations instance to use for handling commands.
     */
    explicit NetRemoteCliHandler(std::unique_ptr<INetRemoteCliHandlerOperationsFactory> operationsFactory);

    /**
     * @brief Assign the parent NetRemoteCli object.
     *
     * @param parent Weak reference to the parent NetRemoteCli object.
     */
    void
    SetParent(std::weak_ptr<NetRemoteCli> parent);

    /**
     * @brief Set the connection object to use.
     *
     * @param connection
     */
    void
    SetConnection(std::shared_ptr<NetRemoteServerConnection> connection);

    /**
     * @brief Handle a command request to enumerate available Wi-Fi access points.
     */
    void
    HandleCommandWifiEnumerateAccessPoints();

    /**
     * @brief Handle a command to enable a Wi-Fi access point.
     *
     * @param accessPointId The identifier of the access point to enable.
     */
    void
    HandleCommandWifiAccessPointEnable(std::string_view accessPointId);

    /**
     * @brief Handle a command to disable a Wi-Fi access point.
     *
     * @param accessPointId The identifier of the access point to enable.
     */
    void
    HandleCommandWifiAccessPointDisable(std::string_view accessPointId);

private:
    /**
     * @brief Obtain a strong reference to the parent NetRemoteCli object. This is used to ensure that the parent object
     * lifetime is extended while handling command execution.
     *
     * @return std::shared_ptr<NetRemoteCli>
     */
    std::shared_ptr<NetRemoteCli>
    GetParentStrongRef() const;

private:
    std::weak_ptr<NetRemoteCli> m_parent;
    std::unique_ptr<INetRemoteCliHandlerOperationsFactory> m_operationsFactory;
    std::shared_ptr<INetRemoteCliHandlerOperations> m_operations;
    std::shared_ptr<NetRemoteServerConnection> m_connection;
};
} // namespace Microsoft::Net::Remote

#endif // NET_REMOTE_CLI_HANDLER_HXX
