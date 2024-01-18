
#ifndef NET_REMOTE_CLI_HANDLER_HXX
#define NET_REMOTE_CLI_HANDLER_HXX

#include <memory>

namespace Microsoft::Net::Remote
{
struct INetRemoteCliHandlerOperations;
struct NetRemoteCli;

/**
 * @brief Handles NetRemote CLI commands.
 */
struct NetRemoteCliHandler
{
    virtual ~NetRemoteCliHandler() = default;

    /**
     * @brief Construct a new NetRemoteCliHandler object.
     *
     * @param operations The operations instance to use for handling commands.
     */
    NetRemoteCliHandler(std::shared_ptr<INetRemoteCliHandlerOperations> operations);

    /**
     * @brief Assign the parent NetRemoteCli object.
     *
     * @param parent Weak reference to the parent NetRemoteCli object.
     */
    void
    SetParent(std::weak_ptr<NetRemoteCli> parent);

    /**
     * @brief Handle a command request to enumerate available Wi-Fi access points.
     */
    void
    HandleCommandWifiEnumerateAccessPoints();

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
    std::shared_ptr<INetRemoteCliHandlerOperations> m_operations;
};
} // namespace Microsoft::Net::Remote

#endif // NET_REMOTE_CLI_HANDLER_HXX
