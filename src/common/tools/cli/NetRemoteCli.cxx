
#include <format>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <CLI/App.hpp>
#include <CLI/Error.hpp>
#include <CLI/Validators.hpp>
#include <microsoft/net/remote/NetRemoteCli.hxx>
#include <microsoft/net/remote/NetRemoteCliData.hxx>
#include <microsoft/net/remote/NetRemoteCliHandler.hxx>
#include <microsoft/net/remote/NetRemoteServerConnection.hxx>
#include <microsoft/net/remote/protocol/NetRemoteProtocol.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <microsoft/net/wifi/Ieee80211AccessPointConfiguration.hxx>
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

    const std::string serverAddressDefault{ Protocol::NetRemoteProtocol::AddressDefault };
    auto* optionServer = app->add_option_function<std::string>("-s,--server", [this](const std::string& serverAddress) {
        OnServerAddressChanged(serverAddress);
    });
    optionServer->description("The address of the netremote server to connect to, with format '<hostname>[:port]'");
    optionServer->default_val(serverAddressDefault)->run_callback_for_default()->force_callback();
    optionServer->default_str(serverAddressDefault);

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

void
NetRemoteCli::WifiAccessPointEnableCallback()
{
    Ieee80211AccessPointConfiguration ieee80211AccessPointConfiguration{};

    if (!std::empty(m_cliData->WifiAccessPointSsid)) {
        ieee80211AccessPointConfiguration.Ssid = m_cliData->WifiAccessPointSsid;
    }
    if (!std::empty(m_cliData->WifiAccessPointFrequencyBands)) {
        ieee80211AccessPointConfiguration.FrequencyBands = m_cliData->WifiAccessPointFrequencyBands;
    }
    if (m_cliData->WifiAccessPointPhyType != Ieee80211PhyType::Unknown) {
        ieee80211AccessPointConfiguration.PhyType = m_cliData->WifiAccessPointPhyType;
    }
    if (m_cliData->WifiAccessPointAuthenticationData.Sae.has_value()) {
        ieee80211AccessPointConfiguration.AuthenticationData.Sae = m_cliData->WifiAccessPointAuthenticationData.Sae.value();
    }

    OnWifiAccessPointEnable(m_cliData->WifiAccessPointId, &ieee80211AccessPointConfiguration);
}

namespace detail
{
const std::map<std::string, Ieee80211PhyType>&
Ieee80211PhyTypeNames()
{
    try {
        static const std::map<std::string, Ieee80211PhyType> ieee80211PhyTypeNames{
            { "a", Ieee80211PhyType::A },
            { "b", Ieee80211PhyType::B },
            { "g", Ieee80211PhyType::G },
            { "n", Ieee80211PhyType::N },
            { "ac", Ieee80211PhyType::AC },
            { "ax", Ieee80211PhyType::AX },
        };
        return ieee80211PhyTypeNames;
    } catch (...) {
        throw std::runtime_error("Failed to create PHY type names");
    }
}

const std::map<std::string, Ieee80211FrequencyBand>&
Ieee80211FrequencyBandNames()
{
    try {
        static const std::map<std::string, Ieee80211FrequencyBand> ieee80211FrequencyBandNames{
            { "2", Ieee80211FrequencyBand::TwoPointFourGHz },
            { "2.4", Ieee80211FrequencyBand::TwoPointFourGHz },
            { "5", Ieee80211FrequencyBand::FiveGHz },
            { "5.0", Ieee80211FrequencyBand::FiveGHz },
            { "6", Ieee80211FrequencyBand::SixGHz },
            { "6.0", Ieee80211FrequencyBand::SixGHz },
        };
        return ieee80211FrequencyBandNames;
    } catch (...) {
        throw std::runtime_error("Failed to create frequency band names");
    }
}

const std::map<std::string, Ieee80211AuthenticationAlgorithm>&
Ieee80211AuthenticationAlgorithmNames()
{
    try {
        static const std::map<std::string, Ieee80211AuthenticationAlgorithm> ieee80211AuthenticationAlgorithmNames{
            { "o", Ieee80211AuthenticationAlgorithm::OpenSystem },
            { "open", Ieee80211AuthenticationAlgorithm::OpenSystem },
            { "open-system", Ieee80211AuthenticationAlgorithm::OpenSystem },
            { "s", Ieee80211AuthenticationAlgorithm::SharedKey },
            { "shared", Ieee80211AuthenticationAlgorithm::SharedKey },
            { "shared-key", Ieee80211AuthenticationAlgorithm::SharedKey },
            { "skey", Ieee80211AuthenticationAlgorithm::SharedKey },
        };
        return ieee80211AuthenticationAlgorithmNames;
    } catch (...) {
        throw std::runtime_error{ "Failed to create authentication algorithm names" };
    }
}

const std::map<std::string, Ieee80211AkmSuite>&
Ieee80211AkmSuiteNames()
{
    try {
        static const std::map<std::string, Ieee80211AkmSuite> ieee80211AkmSuiteNames{
            { "8021x", Ieee80211AkmSuite::Ieee8021x },
            { "dot1x", Ieee80211AkmSuite::Ieee8021x },
            { "psk", Ieee80211AkmSuite::Psk },
            { "ft8021x", Ieee80211AkmSuite::Ft8021x },
            { "ftdot1x", Ieee80211AkmSuite::Ft8021x },
            { "ftpsk", Ieee80211AkmSuite::FtPsk },
            { "8021xsha256", Ieee80211AkmSuite::Ieee8021xSha256 },
            { "dot1xsha256", Ieee80211AkmSuite::Ieee8021xSha256 },
            { "psksha256", Ieee80211AkmSuite::PskSha256 },
            { "tdls", Ieee80211AkmSuite::Tdls },
            { "sae", Ieee80211AkmSuite::Sae },
            { "ftsae", Ieee80211AkmSuite::FtSae },
            { "appeerkey", Ieee80211AkmSuite::ApPeerKey },
            { "8021xsuiteb", Ieee80211AkmSuite::Ieee8021xSuiteB },
            { "8021xsuiteb192", Ieee80211AkmSuite::Ieee8011xSuiteB192 },
            { "dot1xsuiteb", Ieee80211AkmSuite::Ieee8021xSuiteB },
            { "dot1xsuiteb192", Ieee80211AkmSuite::Ieee8011xSuiteB192 },
            { "8021xb", Ieee80211AkmSuite::Ieee8021xSuiteB },
            { "8021xb192", Ieee80211AkmSuite::Ieee8011xSuiteB192 },
            { "dot11b", Ieee80211AkmSuite::Ieee8021xSuiteB },
            { "dot11b192", Ieee80211AkmSuite::Ieee8011xSuiteB192 },
            { "ft8021xsha384", Ieee80211AkmSuite::Ft8021xSha384 },
            { "ftdot1xsha384", Ieee80211AkmSuite::Ft8021xSha384 },
            { "filssha256", Ieee80211AkmSuite::FilsSha256 },
            { "filssha384", Ieee80211AkmSuite::FilsSha384 },
            { "ftfilssha256", Ieee80211AkmSuite::FtFilsSha256 },
            { "ftfilssha384", Ieee80211AkmSuite::FtFilsSha384 },
            { "owe", Ieee80211AkmSuite::Owe },
            { "ftpsksha384", Ieee80211AkmSuite::FtPskSha384 },
            { "psksha384", Ieee80211AkmSuite::PskSha384 },
            { "pasn", Ieee80211AkmSuite::Pasn },
        };
        return ieee80211AkmSuiteNames;
    } catch (...) {
        throw std::runtime_error{ "Failed to create AKM suite names" };
    }
}
} // namespace detail

CLI::App*
NetRemoteCli::AddSubcommandWifiAccessPointEnable(CLI::App* parent)
{
    auto* cliAppWifiAccessPointEnable = parent->add_subcommand("access-point-enable", "Enable a Wi-Fi access point");
    cliAppWifiAccessPointEnable->alias("ap-enable")->alias("enable")->alias("ape");
    cliAppWifiAccessPointEnable->add_option("id", m_cliData->WifiAccessPointId, "The identifier of the access point to enable")->required();
    cliAppWifiAccessPointEnable->add_option("--ssid", m_cliData->WifiAccessPointSsid, "The SSID of the access point to enable");
    cliAppWifiAccessPointEnable->add_option("--phy,--phyType,", m_cliData->WifiAccessPointPhyType, "The PHY type of the access point to enable")
        ->transform(CLI::CheckedTransformer(detail::Ieee80211PhyTypeNames(), CLI::ignore_case));
    cliAppWifiAccessPointEnable->add_option("--freq,--freqs,--frequencies,--frequencyBand,--frequencyBands,--band,--bands", m_cliData->WifiAccessPointFrequencyBands, "The frequency bands of the access point to enable")
        ->transform(CLI::CheckedTransformer(detail::Ieee80211FrequencyBandNames()));
    cliAppWifiAccessPointEnable->add_option("--auth,--auths,--authAlg,--authAlgs,--authentication,--authenticationAlgorithm,--authenticationAlgorithms", m_cliData->WifiAccessPointAuthenticationAlgorithms, "The authentication algorithms of the access point to enable")
        ->transform(CLI::CheckedTransformer(detail::Ieee80211AuthenticationAlgorithmNames(), CLI::ignore_case));
    cliAppWifiAccessPointEnable->add_option("--akm,--akms,--akmSuite,--akmSuites,--keyManagement,--keyManagements", m_cliData->WifiAccessPointAkmSuites, "The AKM suites of the access point to enable")
        ->transform(CLI::CheckedTransformer(detail::Ieee80211AkmSuiteNames(), CLI::ignore_case));
    cliAppWifiAccessPointEnable->callback([this] {
        WifiAccessPointEnableCallback();
    });

    return cliAppWifiAccessPointEnable;
}

CLI::App*
NetRemoteCli::AddSubcommandWifiAccessPointDisable(CLI::App* parent)
{
    auto* cliAppWifiAccessPointDisable = parent->add_subcommand("access-point-disable", "Disable a Wi-Fi access point");
    cliAppWifiAccessPointDisable->alias("ap-disable")->alias("disable")->alias("apd");
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

    LOGI << std::format("Connecting to server {}", serverAddress);
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
