
#ifndef NET_REMOTE_CLI_HANDLER_HXX
#define NET_REMOTE_CLI_HANDLER_HXX

#include <memory>
#include <string_view>
#include <tuple>

#include <microsoft/net/Ieee8021xRadiusAuthentication.hxx>
#include <microsoft/net/remote/NetRemoteServerConnection.hxx>
#include <microsoft/net/wifi/Ieee80211AccessPointConfiguration.hxx>

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
     * @brief Handle a command request to enumerate available network interfaces.
     */
    void
    HandleCommandNetworkInterfacesEnumerate();

    /**
     * @brief Handle a command request to enumerate available Wi-Fi access points.
     *
     * @param detailedOutput Whether the output should be detailed (false) or brief (true, single line).
     */
    void
    HandleCommandWifiAccessPointsEnumerate(bool detailedOutput);

    /**
     * @brief Handle a command to enable a Wi-Fi access point.
     *
     * @param accessPointId The identifier of the access point to enable.
     * @param ieee80211AccessPointConfiguration The optional configuration to apply to the access point.
     */
    void
    HandleCommandWifiAccessPointEnable(std::string_view accessPointId, const Microsoft::Net::Wifi::Ieee80211AccessPointConfiguration* ieee80211AccessPointConfiguration = nullptr);

    /**
     * @brief Handle a command to disable a Wi-Fi access point.
     *
     * @param accessPointId The identifier of the access point to enable.
     */
    void
    HandleCommandWifiAccessPointDisable(std::string_view accessPointId);

    /**
     * @brief Handle a command to set the SSID of a Wi-Fi access point.
     *
     * @param accessPointId The identifier of the access point to set the SSID for.
     * @param ssid The SSID to set.
     */
    void
    HandleCommandWifiAccessPointSetSsid(std::string_view accessPointId, std::string_view ssid);

    /**
     * @brief Handle a command to set the RADIUS configuration for a Wi-Fi access point.
     *
     * @param accessPointId The identifier of the access point to set the RADIUS configuration for.
     * @param ieee8021xRadiusConfiguration The RADIUS authentication configuration to set.
     */
    void
    HandleCommandWifiAccessPointSet8021xRadius(std::string_view accessPointId, const Microsoft::Net::Ieee8021xRadiusConfiguration* ieee8021xRadiusConfiguration);

private:
    /**
     * @brief Obtain a strong reference to the parent NetRemoteCli object. This is used to ensure that the parent object
     * lifetime is extended while handling command execution.
     *
     * @return std::shared_ptr<NetRemoteCli>
     */
    std::shared_ptr<NetRemoteCli>
    GetParentStrongRef() const;

    /**
     * @brief Obtain a strong reference to the parent NetRemoteCli object and the operations instance to use for
     * handling commands.
     * 
     * @return std::tuple<std::shared_ptr<NetRemoteCli>, std::shared_ptr<INetRemoteCliHandlerOperations>> 
     */
    std::tuple<std::shared_ptr<NetRemoteCli>, std::shared_ptr<INetRemoteCliHandlerOperations>>
    GetOperationsAndParentStrongRef() const;

private:
    std::weak_ptr<NetRemoteCli> m_parent;
    std::unique_ptr<INetRemoteCliHandlerOperationsFactory> m_operationsFactory;
    std::shared_ptr<INetRemoteCliHandlerOperations> m_operations;
    std::shared_ptr<NetRemoteServerConnection> m_connection;
};
} // namespace Microsoft::Net::Remote

#endif // NET_REMOTE_CLI_HANDLER_HXX
