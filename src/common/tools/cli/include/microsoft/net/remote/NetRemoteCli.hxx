
#ifndef NET_REMOTE_CLI_HXX
#define NET_REMOTE_CLI_HXX

#include <memory>

#include <CLI/CLI.hpp>
#include <microsoft/net/remote/NetRemoteCliData.hxx>
#include <microsoft/net/remote/NetRemoteCliHandler.hxx>

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
    Create(std::shared_ptr<NetRemoteCliData> cliData, std::shared_ptr<NetRemoteCliHandler> cliHandler);

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
    Parse(int argc, char* argv[]) noexcept;

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

private:
    std::shared_ptr<NetRemoteCliData> m_cliData;
    std::shared_ptr<NetRemoteCliHandler> m_cliHandler;
    std::unique_ptr<CLI::App> m_cliApp;
};
} // namespace Microsoft::Net::Remote

#endif // NET_REMOTE_CLI_HXX
