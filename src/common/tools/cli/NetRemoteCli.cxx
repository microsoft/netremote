
#include <format>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include <CLI/App.hpp>
#include <CLI/Error.hpp>
#include <CLI/Validators.hpp>
#include <microsoft/net/remote/NetRemoteCli.hxx>
#include <microsoft/net/remote/NetRemoteCliData.hxx>
#include <microsoft/net/remote/NetRemoteCliHandler.hxx>
#include <microsoft/net/remote/NetRemoteServerConnection.hxx>
#include <microsoft/net/remote/protocol/NetRemoteProtocol.hxx>
#include <notstd/Memory.hxx>
#include <plog/Log.h>

using namespace Microsoft::Net::Remote;
using namespace Microsoft::Net::Wifi;

NetRemoteCli::NetRemoteCli(std::shared_ptr<NetRemoteCliData> cliData, std::shared_ptr<NetRemoteCliHandler> cliHandler) :
    m_cliData{ std::move(cliData) },
    m_cliHandler{ std::move(cliHandler) },
    m_cliApp{ CreateParser() }
{
}

/* static */
std::shared_ptr<NetRemoteCli>
NetRemoteCli::Create(std::shared_ptr<NetRemoteCliData> cliData, const std::shared_ptr<NetRemoteCliHandler>& cliHandler)
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
NetRemoteCli::Parse(int argc, char* argv[]) noexcept // NOLINT(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
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

    auto* optionServer = app->add_option_function<std::string>("-s,--server", [this](const std::string& serverAddress) {
        OnServerAddressChanged(serverAddress);
    });
    optionServer->description("The address of the netremote server to connect to, with format '<hostname>[:port]");

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
    m_cliAppWifiAccessPointsEnumerate = AddSubcommandWifiAccessPointsEnumerate(wifiApp);
    m_cliAppWifiAccessPointEnable = AddSubcommandWifiAccessPointEnable(wifiApp);
    m_cliAppWifiAccessPointDisable = AddSubcommandWifiAccessPointDisable(wifiApp);

    return wifiApp;
}

CLI::App*
NetRemoteCli::AddSubcommandWifiAccessPointsEnumerate(CLI::App* parent)
{
    auto* cliAppWifiAccessPointsEnumerate = parent->add_subcommand("enumerate-access-points", "Enumerate available Wi-Fi access points");
    cliAppWifiAccessPointsEnumerate->add_flag("--detailed", m_cliData->DetailedOutput, "Show detailed information about each access point");
    cliAppWifiAccessPointsEnumerate->alias("enumaps")->alias("enum")->alias("aps");
    cliAppWifiAccessPointsEnumerate->callback([this] {
        OnWifiAccessPointsEnumerate(m_cliData->DetailedOutput.value_or(false));
    });

    return cliAppWifiAccessPointsEnumerate;
}

CLI::App*
NetRemoteCli::AddSubcommandWifiAccessPointEnable(CLI::App* parent)
{
    const std::map<std::string, Ieee80211PhyType> Ieee80211PhyTypeNames{
        { "a", Ieee80211PhyType::A },
        { "b", Ieee80211PhyType::B },
        { "g", Ieee80211PhyType::G },
        { "n", Ieee80211PhyType::N },
        { "ac", Ieee80211PhyType::AC },
        { "ax", Ieee80211PhyType::AX },
    };

    auto* cliAppWifiAccessPointEnable = parent->add_subcommand("access-point-enable", "Enable a Wi-Fi access point");
    cliAppWifiAccessPointEnable->alias("ap-enable")->alias("enable")->alias("ap-en");
    cliAppWifiAccessPointEnable->callback([this] {
        Ieee80211AccessPointConfiguration ieee80211AccessPointConfiguration{};
        if (!std::empty(m_cliData->WifiAccessPointSsid)) {
            ieee80211AccessPointConfiguration.Ssid = m_cliData->WifiAccessPointSsid;
        }
        if (m_cliData->WifiAccessPointPhyType != Ieee80211PhyType::Unknown) {
            ieee80211AccessPointConfiguration.PhyType = m_cliData->WifiAccessPointPhyType;
        }

        OnWifiAccessPointEnable(m_cliData->WifiAccessPointId, &ieee80211AccessPointConfiguration);
    });

    cliAppWifiAccessPointEnable->add_option("id", m_cliData->WifiAccessPointId, "The identifier of the access point to enable")->required();
    cliAppWifiAccessPointEnable->add_option("--ssid", m_cliData->WifiAccessPointSsid, "The SSID of the access point to enable");
    cliAppWifiAccessPointEnable->add_option("--phy,--phyType,", m_cliData->WifiAccessPointPhyType, "The PHY type of the access point to enable")
        ->transform(CLI::CheckedTransformer(Ieee80211PhyTypeNames, CLI::ignore_case));

    return cliAppWifiAccessPointEnable;
}

CLI::App*
NetRemoteCli::AddSubcommandWifiAccessPointDisable(CLI::App* parent)
{
    auto* cliAppWifiAccessPointDisable = parent->add_subcommand("access-point-disable", "Disable a Wi-Fi access point");
    cliAppWifiAccessPointDisable->alias("ap-disable")->alias("disable")->alias("ap-dis");
    cliAppWifiAccessPointDisable->callback([this] {
        OnWifiAccessPointDisable(m_cliData->WifiAccessPointId);
    });

    cliAppWifiAccessPointDisable->add_option("id", m_cliData->WifiAccessPointId, "The identifier of the access point to disable")->required();

    return cliAppWifiAccessPointDisable;
}

void
NetRemoteCli::OnServerAddressChanged(const std::string& serverAddressArg)
{
    using Protocol::NetRemoteProtocol;

    // Append the default port if not specified in command-line argument.
    auto serverAddress = serverAddressArg;
    if (serverAddress.find(':') == std::string::npos) {
        serverAddress += std::format("{}{}", NetRemoteProtocol::PortSeparator, NetRemoteProtocol::PortDefault);
    }

    m_cliData->ServerAddress = std::move(serverAddress);

    auto connection = NetRemoteServerConnection::TryEstablishConnection(m_cliData->ServerAddress);
    if (connection == nullptr) {
        LOGE << "Failed to create server connection";
        return;
    }

    m_cliHandler->SetConnection(std::move(connection));
}

void
NetRemoteCli::OnWifiAccessPointsEnumerate(bool detailedOutput)
{
    m_cliHandler->HandleCommandWifiAccessPointsEnumerate(detailedOutput);
}

void
NetRemoteCli::OnWifiAccessPointEnable(std::string_view accessPointId, const Ieee80211AccessPointConfiguration* ieee80211AccessPointConfiguration)
{
    m_cliHandler->HandleCommandWifiAccessPointEnable(accessPointId, ieee80211AccessPointConfiguration);
}

void
NetRemoteCli::OnWifiAccessPointDisable(std::string_view accessPointId)
{
    m_cliHandler->HandleCommandWifiAccessPointDisable(accessPointId);
}
