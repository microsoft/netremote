
#include <string>

#include <microsoft/net/remote/NetRemoteCli.hxx>
#include <notstd/Memory.hxx>
#include <plog/Log.h>

using namespace Microsoft::Net::Remote;

NetRemoteCli::NetRemoteCli(std::shared_ptr<NetRemoteCliData> cliData, std::shared_ptr<NetRemoteCliHandler> cliHandler) :
    m_cliData{ cliData },
    m_cliHandler{ cliHandler },
    m_cliApp{ CreateParser() }
{
}

/* static */
std::shared_ptr<NetRemoteCli>
NetRemoteCli::Create(std::shared_ptr<NetRemoteCliData> cliData, std::shared_ptr<NetRemoteCliHandler> cliHandler)
{
    auto instance = std::make_shared<notstd::enable_make_protected<NetRemoteCli>>(std::move(cliData), cliHandler);
    cliHandler->SetParent(instance->weak_from_this());

    return std::move(instance);
}

CLI::App&
NetRemoteCli::GetParser()
{
    return *m_cliApp;
}

std::shared_ptr<NetRemoteCliData>
NetRemoteCli::GetData() const
{
    return m_cliData;
}

int
NetRemoteCli::Parse(int argc, char* argv[]) noexcept
{
    try {
        m_cliApp->parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        return m_cliApp->exit(e);
    } catch (...) {
        LOGF << "Unhandled exception";
    }

    return 0;
}

std::unique_ptr<CLI::App>
NetRemoteCli::CreateParser() noexcept
{
    auto app = std::make_unique<CLI::App>("A command line tool to interact with netremote servers", "netremote-cli");

    app->require_subcommand();

    auto optionServer = app->add_option_function<std::string>("-s,--server", [this](const std::string& serverAddress) {
        OnServerAddressChanged(serverAddress);
    });
    optionServer->description("The address of the netremote server to connect to");

    m_cliAppServerAddress = optionServer;
    m_cliAppWifi = AddSubcommandWifi(app.get());

    return app;
}

CLI::App*
NetRemoteCli::AddSubcommandWifi(CLI::App* parent)
{
    // Top-level command.
    auto* wifiApp = parent->add_subcommand("wifi", "Wi-Fi operations");
    wifiApp->needs(m_cliAppServerAddress);

    // Sub-commands.
    m_cliAppWifiEnumerateAccessPoints = AddSubcommandWifiEnumerateAccessPoints(wifiApp);

    return wifiApp;
}

CLI::App*
NetRemoteCli::AddSubcommandWifiEnumerateAccessPoints(CLI::App* parent)
{
    auto cliAppWifiEnumerateAccessPoints = parent->add_subcommand("enumerate-access-points", "Enumerate available Wi-Fi access points");
    cliAppWifiEnumerateAccessPoints->alias("enumaps");
    cliAppWifiEnumerateAccessPoints->callback([this] {
        OnWifiEnumerateAccessPoints();
    });

    return cliAppWifiEnumerateAccessPoints;
}

void
NetRemoteCli::OnServerAddressChanged(const std::string& serverAddress)
{
    m_cliData->ServerAddress = serverAddress;

    auto connection = NetRemoteServerConnection::TryEstablishConnection(serverAddress);
    if (connection == nullptr) {
        LOGE << "Failed to create server connection";
        return;
    }

    m_cliHandler->SetConnection(std::move(connection));
}

void
NetRemoteCli::OnWifiEnumerateAccessPoints()
{
    m_cliHandler->HandleCommandWifiEnumerateAccessPoints();
}
