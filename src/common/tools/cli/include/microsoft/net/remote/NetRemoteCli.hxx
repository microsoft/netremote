
#ifndef NET_REMOTE_CLI_HXX
#define NET_REMOTE_CLI_HXX

#include <memory>

#include <CLI/CLI.hpp>
#include <microsoft/net/remote/NetRemoteCliData.hxx>
#include <microsoft/net/remote/NetRemoteCliHandler.hxx>
#include <microsoft/net/remote/NetRemoteServerConnection.hxx>
#include <microsoft/net/wifi/Ieee80211AccessPointConfiguration.hxx>

namespace Microsoft::Net::Remote
{
/**
 * @brief Netremote command-line top-level object.
 *
 * This collects command-line interface functionality for the netremote command-line tool, including parsing options and
 * driving execution of the requested operations.
 */
struct NetRemoteCli :
    public std::enable_shared_from_this<NetRemoteCli>
{
    /**
     * @brief Construct a new NetRemoteCli object.
     *
     * @param cliData An instance of the parsed command-line data/arguments.
     * @param cliHandler An instance to handle parsed command requests.
     * @return std::shared_ptr<NetRemoteClie>
     */
    static std::shared_ptr<NetRemoteCli>
    Create(std::shared_ptr<NetRemoteCliData> cliData, const std::shared_ptr<NetRemoteCliHandler>& cliHandler);

    /**
     * @brief Get a reference to the parser. The parser will be configured with all common command-line interface
     * options and flags, and may be extended further for the specific context of the CLI.
     *
     * @return CLI::App&
     */
    CLI::App&
    GetParser();

    /**
     * @brief Get the data object.
     *
     * @return std::shared_ptr<NetRemoteCliData>
     */
    std::shared_ptr<NetRemoteCliData>
    GetData() const;

    /**
     * @brief Parse the specified command line arguments.
     *
     * @param argc
     * @param argv
     * @return int
     */
    int
    Parse(int argc, char* argv[]) noexcept; // NOLINT(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays, hicpp-avoid-c-arrays)

protected:
    /**
     * @brief Construct a new NetRemoteCli object.
     *
     * @param cliData An instance of the parsed command-line data/arguments.
     * @param cliHandler An instance to handle parsed command requests.
     */
    NetRemoteCli(std::shared_ptr<NetRemoteCliData> cliData, std::shared_ptr<NetRemoteCliHandler> cliHandler);

private:
    /**
     * @brief Create an initialize the command line parser. This will create a
     * parser and configure it with all common options and flags.
     *
     * @return std::unique_ptr<CLI::App>
     */
    std::unique_ptr<CLI::App>
    CreateParser() noexcept;

    /**
     * @brief Add the 'net' sub-command.
     *
     * @param parent The parent app to add the sub-command to.
     * @return CLI::App*
     */
    CLI::App*
    AddSubcommandNetwork(CLI::App* parent);

    /**
     * @brief Add the 'net enumerate-interfaces' sub-command.
     *
     * @param parent The parent app to add the sub-command to.
     * @return CLI::App*
     */
    CLI::App*
    AddSubcommandNetworkEnumerateInterfaces(CLI::App* parent);

    /**
     * @brief Add the 'wifi' sub-command.
     *
     * @param parent The parent app to add the sub-command to.
     * @return CLI::App*
     */
    CLI::App*
    AddSubcommandWifi(CLI::App* parent);

    /**
     * @brief Add the 'wifi enumerate-access-points' sub-command.
     *
     * @param parent The parent app to add the sub-command to.
     * @return CLI::App*
     */
    CLI::App*
    AddSubcommandWifiAccessPointsEnumerate(CLI::App* parent);

    /**
     * @brief Add the 'wifi ap-enable' sub-command.
     *
     * @param parent The parent app to add the sub-command to.
     * @return CLI::App*
     */
    CLI::App*
    AddSubcommandWifiAccessPointEnable(CLI::App* parent);

    /**
     * @brief Add the 'wifi ap-disable' sub-command.
     *
     * @param parent The parent app to add the sub-command to.
     * @return CLI::App*
     */
    CLI::App*
    AddSubcommandWifiAccessPointDisable(CLI::App* parent);

    /**
     * @brief Add the 'wifi ap-set-ssid' sub-command.
     * 
     * @param parent The parent app to add the sub-command to.
     * @return CLI::App* 
     */
    CLI::App*
    AddSubcommandWifiAccessPointSetSsid(CLI::App* parent);

    /**
     * @brief Handle the 'server' option.
     *
     * @param serverAddress The server address to connect to.
     */
    void
    OnServerAddressChanged(const std::string& serverAddress);

    /**
     * @brief Handle the 'net enumerate-interfaces' command.
     */
    void
    OnNetworkInterfacesEnumerate();

    /**
     * @brief Handle the 'wifi enumerate-access-points' command.
     *
     * @param detailedOutput Whether the output should be detailed (false) or brief (true, single line).
     */
    void
    OnWifiAccessPointsEnumerate(bool detailedOutput = false);

    /**
     * @brief Invoked when all processing for the 'wifi ap-enable' command is complete.
     */
    void
    WifiAccessPointEnableCallback();

    /**
     * @brief Handle the 'wifi ap-enable' command.
     *
     * @param accessPointId The identifier of the access point to enable.
     * @param ieee80211AccessPointConfiguration Optional configuration for the access point to enable.
     */
    void
    OnWifiAccessPointEnable(std::string_view accessPointId, const Microsoft::Net::Wifi::Ieee80211AccessPointConfiguration* ieee80211AccessPointConfiguration = nullptr);

    /**
     * @brief Handle the 'wifi ap-set-ssid' command.
     * 
     * @param accessPointId The identifier of the access point to set the SSID for.
     * @param ssid The SSID to set.
     */
    void
    OnWifiAccessPointSetSsid(std::string_view accessPointId, std::string_view ssid);

    /**
     * @brief Handle the 'wifi ap-disable' command.
     *
     * @param accessPointId The identifier of the access point to disable.
     */
    void
    OnWifiAccessPointDisable(std::string_view accessPointId);

private:
    std::shared_ptr<NetRemoteCliData> m_cliData;
    std::shared_ptr<NetRemoteCliHandler> m_cliHandler;
    std::unique_ptr<CLI::App> m_cliApp;
    std::shared_ptr<NetRemoteServerConnection> m_connection;

    // The following are helper references to the subcommands of m_cliApp; the memory is managed by CLI11.
    CLI::Option* m_cliAppServerAddress{ nullptr };
    CLI::App* m_cliAppNetwork{ nullptr };
    CLI::App* m_cliAppNetworkEnumerateInterfaces{ nullptr };
    CLI::App* m_cliAppWifi{ nullptr };
    CLI::App* m_cliAppWifiAccessPointsEnumerate{ nullptr };
    CLI::App* m_cliAppWifiAccessPointEnable{ nullptr };
    CLI::App* m_cliAppWifiAccessPointDisable{ nullptr };
    CLI::App* m_cliAppWifiAccessPointSetSsid{ nullptr };
};
} // namespace Microsoft::Net::Remote

#endif // NET_REMOTE_CLI_HXX
