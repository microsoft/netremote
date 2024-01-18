
#ifndef I_NET_REMOTE_CLI_HANDLER_OPERATIONS_HXX
#define I_NET_REMOTE_CLI_HANDLER_OPERATIONS_HXX

namespace Microsoft::Net::Remote
{
/**
 * @brief Operations that can be carried out by a NetRemote CLI handler.
 *
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
} // namespace Microsoft::Net::Remote

#endif // I_NET_REMOTE_CLI_HANDLER_OPERATIONS_HXX
