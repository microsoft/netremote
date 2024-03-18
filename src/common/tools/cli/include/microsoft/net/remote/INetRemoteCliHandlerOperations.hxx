
#ifndef I_NET_REMOTE_CLI_HANDLER_OPERATIONS_HXX
#define I_NET_REMOTE_CLI_HANDLER_OPERATIONS_HXX

#include <memory>
#include <optional>
#include <string_view>

#include <microsoft/net/remote/NetRemoteServerConnection.hxx>
#include <microsoft/net/wifi/Ieee80211AccessPointConfiguration.hxx>

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
     *
     * @param detailedOutput Whether the output should be detailed (false) or brief (true, single line).
     */
    virtual void
    WifiEnumerateAccessPoints(bool detailedOutput) = 0;

    /**
     * @brief Enable the specified WiFi access point.
     *
     * @param accessPointId The identifier of the access point to enable.
     * @param ieee80211AccessPointConfiguration The optional configuration to apply to the access point.
     */
    virtual void
    WifiAccessPointEnable(std::string_view accessPointId, const std::optional<Microsoft::Net::Wifi::Ieee80211AccessPointConfiguration>& ieee80211AccessPointConfiguration) = 0;

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
