
#include <format>
#include <iostream>
#include <istream>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <CLI/App.hpp>
#include <CLI/Error.hpp>
#include <CLI/Validators.hpp>
#include <microsoft/net/Ieee8021xRadiusAuthentication.hxx>
#include <microsoft/net/NetworkIpAddress.hxx>
#include <microsoft/net/remote/NetRemoteCli.hxx>
#include <microsoft/net/remote/NetRemoteCliData.hxx>
#include <microsoft/net/remote/NetRemoteCliHandler.hxx>
#include <microsoft/net/remote/NetRemoteServerConnection.hxx>
#include <microsoft/net/remote/protocol/NetRemoteProtocol.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <microsoft/net/wifi/Ieee80211AccessPointConfiguration.hxx>
#include <notstd/Memory.hxx>
#include <plog/Log.h>
#include <strings/StringParsing.hxx>

using namespace Microsoft::Net;
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
    m_cliAppNetwork = AddSubcommandNetwork(app.get());
    m_cliAppWifi = AddSubcommandWifi(app.get());

    return app;
}

CLI::App*
NetRemoteCli::AddSubcommandNetwork(CLI::App* parent)
{
    // Top-level command.
    auto* networkApp = parent->add_subcommand("net", "Network operations");
    networkApp->needs(m_cliAppServerAddress);

    // Sub-commands.
    m_cliAppNetworkEnumerateInterfaces = AddSubcommandNetworkEnumerateInterfaces(networkApp);

    return networkApp;
}

CLI::App*
NetRemoteCli::AddSubcommandNetworkEnumerateInterfaces(CLI::App* parent)
{
    auto* cliAppNetworkEnumerateInterfaces = parent->add_subcommand("enumerate-interfaces", "Enumerate available network interfaces");
    cliAppNetworkEnumerateInterfaces->alias("enumifs")->alias("enum")->alias("interfaces")->alias("ifaces")->alias("ifs");
    cliAppNetworkEnumerateInterfaces->callback([this] {
        OnNetworkInterfacesEnumerate();
    });

    return cliAppNetworkEnumerateInterfaces;
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
    m_cliAppWifiAccessPointSetSsid = AddSubcommandWifiAccessPointSetSsid(wifiApp);
    m_cliAppWifiAccessPointSetAuthenticationDot1x = AddSubcommandWifiAccessPointSet8021xRadius(wifiApp);

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

namespace detail
{
/**
 * @brief Thin wrapper to destructure the std::tuple used to parse SAE passwords.
 *
 * @param saePasswordArgument The tuple containing the SAE password, password ID, and peer MAC address.
 * @return Ieee80211RsnaPassword
 */
Ieee80211RsnaPassword
ParseSaePasswordCliArgument(std::tuple<std::string, std::optional<std::string>, std::optional<std::string>>& saePasswordArgument)
{
    const auto& [password, passwordId, peerMacAddress] = saePasswordArgument; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)

    Ieee80211RsnaPassword saePassword{
        .Credential = std::move(password),
        .PasswordId = std::move(passwordId),
        .PeerMacAddress = peerMacAddress.and_then(Ieee80211MacAddressFromString)
    };

    return saePassword;
}
} // namespace detail

void
NetRemoteCli::WifiAccessPointEnableCallback()
{
    Ieee80211AccessPointConfiguration ieee80211AccessPointConfiguration{};

    if (!std::empty(m_cliData->WifiAccessPointPskPassphrase)) {
        auto& psk = ieee80211AccessPointConfiguration.AuthenticationData.Psk.emplace();
        psk.Psk = m_cliData->WifiAccessPointPskPassphrase;
    }
    if (!std::empty(m_cliData->WifiAccessPointPskHex)) {
        auto& psk = ieee80211AccessPointConfiguration.AuthenticationData.Psk.emplace();
        auto& pskValue = psk.Psk.emplace<Ieee80211RsnaPskValue>();
        if (!Strings::ParseHexString(m_cliData->WifiAccessPointPskHex, pskValue)) {
            std::cerr << "Failed to parse PSK value (must be 64 hex characters)" << std::endl;
            return;
        }
    }

    if (!std::empty(m_cliData->WifiAccessPointSaePasswords)) {
        auto& sae = ieee80211AccessPointConfiguration.AuthenticationData.Sae.emplace();
        sae.Passwords.resize(std::size(m_cliData->WifiAccessPointSaePasswords));
        std::ranges::transform(m_cliData->WifiAccessPointSaePasswords, std::begin(sae.Passwords), detail::ParseSaePasswordCliArgument);
    }

    if (!std::empty(m_cliData->WifiAccessPointSsid)) {
        ieee80211AccessPointConfiguration.Ssid = m_cliData->WifiAccessPointSsid;
    }
    if (!std::empty(m_cliData->WifiAccessPointFrequencyBands)) {
        ieee80211AccessPointConfiguration.FrequencyBands = m_cliData->WifiAccessPointFrequencyBands;
    }
    if (m_cliData->WifiAccessPointPhyType != Ieee80211PhyType::Unknown) {
        ieee80211AccessPointConfiguration.PhyType = m_cliData->WifiAccessPointPhyType;
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
    cliAppWifiAccessPointEnable->add_option("id,--id", m_cliData->WifiAccessPointId, "The identifier of the access point to enable")->required();
    cliAppWifiAccessPointEnable->add_option("--ssid", m_cliData->WifiAccessPointSsid, "The SSID of the access point to enable");
    cliAppWifiAccessPointEnable->add_option("--phy,--phyType,", m_cliData->WifiAccessPointPhyType, "The PHY type of the access point to enable")
        ->transform(CLI::CheckedTransformer(detail::Ieee80211PhyTypeNames(), CLI::ignore_case));
    cliAppWifiAccessPointEnable->add_option("--freq,--freqs,--frequencies,--frequencyBand,--frequencyBands,--band,--bands", m_cliData->WifiAccessPointFrequencyBands, "The frequency bands of the access point to enable")
        ->transform(CLI::CheckedTransformer(detail::Ieee80211FrequencyBandNames()));
    cliAppWifiAccessPointEnable->add_option("--auth,--auths,--authAlg,--authAlgs,--authentication,--authenticationAlgorithm,--authenticationAlgorithms", m_cliData->WifiAccessPointAuthenticationAlgorithms, "The authentication algorithms of the access point to enable")
        ->transform(CLI::CheckedTransformer(detail::Ieee80211AuthenticationAlgorithmNames(), CLI::ignore_case));
    cliAppWifiAccessPointEnable->add_option("--akm,--akms,--akmSuite,--akmSuites,--keyManagement,--keyManagements", m_cliData->WifiAccessPointAkmSuites, "The AKM suites of the access point to enable")
        ->transform(CLI::CheckedTransformer(detail::Ieee80211AkmSuiteNames(), CLI::ignore_case));
    cliAppWifiAccessPointEnable->add_option("--psk", m_cliData->WifiAccessPointPskHex, "The PSK of the access point to enable");
    cliAppWifiAccessPointEnable->add_option("--passphrase,--pskPassphrase", m_cliData->WifiAccessPointPskPassphrase, "The PSK passphrase of the access point to enable")->excludes("--psk");
    cliAppWifiAccessPointEnable->add_option("--sae,--password,--passwords,--saePassword,--saePasswords", m_cliData->WifiAccessPointSaePasswords, "The SAE passwords of the access point to enable")->type_size(1, 3);
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

CLI::App*
NetRemoteCli::AddSubcommandWifiAccessPointSetSsid(CLI::App* parent)
{
    auto* cliAppWifiAccessPointSetSsid = parent->add_subcommand("access-point-set-ssid", "Set the SSID of an access point");
    cliAppWifiAccessPointSetSsid->alias("ap-set-ssid")->alias("set-ssid")->alias("setssid")->alias("ssid");
    cliAppWifiAccessPointSetSsid->callback([this] {
        OnWifiAccessPointSetSsid(m_cliData->WifiAccessPointId, m_cliData->WifiAccessPointSsid);
    });

    cliAppWifiAccessPointSetSsid->add_option("id", m_cliData->WifiAccessPointId, "The identifier of the access point to set the SSID for")->required();
    cliAppWifiAccessPointSetSsid->add_option("ssid,--ssid", m_cliData->WifiAccessPointSsid, "The SSID of the access point to enable")->required();

    return cliAppWifiAccessPointSetSsid;
}

namespace Microsoft::Net
{
/**
 * @brief Convert an Ieee8021xRadiusServerEndpointType to a short string representation.
 *
 * @param ieee8021xRadiusServerEndpointType The Ieee8021xRadiusServerEndpointType to convert.
 * @return constexpr std::string_view A short string representation of the Ieee8021xRadiusServerEndpointType.
 */
constexpr std::string_view
ToString(Ieee8021xRadiusServerEndpointType ieee8021xRadiusServerEndpointType) noexcept
{
    switch (ieee8021xRadiusServerEndpointType) {
    case Ieee8021xRadiusServerEndpointType::Authentication:
        return "auth";
    case Ieee8021xRadiusServerEndpointType::Accounting:
        return "acct";
    default:
        return "????";
    }
}

/**
 * @brief Convert a Ieee8021xRadiusServerEndpointType to a human-readable string representation.
 *
 * @param ieee8021xRadiusServerEndpointConfiguration The Ieee8021xRadiusServerEndpointConfiguration to convert.
 * @return std::string A human-readable string representation of the Ieee8021xRadiusServerEndpointConfiguration.
 */
std::string
ToString(const Ieee8021xRadiusServerEndpointConfiguration& ieee8021xRadiusServerEndpointConfiguration) noexcept
{
    return std::format("[{}] {}:{} secret='{}'",
        ToString(ieee8021xRadiusServerEndpointConfiguration.Type),
        ieee8021xRadiusServerEndpointConfiguration.Address,
        ieee8021xRadiusServerEndpointConfiguration.Port.value_or(0),
        std::string(std::cbegin(ieee8021xRadiusServerEndpointConfiguration.SharedSecret), std::cend(ieee8021xRadiusServerEndpointConfiguration.SharedSecret)));
}

/**
 * @brief Convert an Ieee8021xRadiusServerEndpointType to a string representation.
 *
 * @param ieee8021xRadiusConfiguration The Ieee8021xRadiusServerEndpointConfiguration to convert.
 * @return std::string A multi-line string representation of the Ieee8021xRadiusServerEndpointConfiguration.
 */
std::string
ToString(const Ieee8021xRadiusConfiguration& ieee8021xRadiusConfiguration) noexcept
{
    std::ostringstream output{};
    output << " " << ToString(ieee8021xRadiusConfiguration.AuthenticationServer) << " (primary)\n"
           << " " << (ieee8021xRadiusConfiguration.AccountingServer.has_value() ? ToString(ieee8021xRadiusConfiguration.AccountingServer.value()) : "[Accounting] <none>") << " (primary)\n";
    for (const auto& fallbackServer : ieee8021xRadiusConfiguration.FallbackServers) {
        output << " " << ToString(fallbackServer) << " (fallback)\n";
    }

    return output.str();
}

/**
 * @brief Attempt to parse a vector of tokens into an Ieee8021xRadiusServerEndpointConfiguration.
 *
 * The expected format of the tokens is: [<[auth|acct]>,]<address>[:<port>],<sharedsecret>
 *
 * If the type is not specified, it defaults to 'auth' (authentication).
 * If the port is not specified, it defaults to 0, which will not set the 'Port' field.
 *
 * @param ieee8021xRadiusServerEndpointConfigurationTokens The string tokens to parse into an Ieee8021xRadiusServerEndpointConfiguration.
 * @param ieee8021xRadiusServerEndpointConfiguration The Ieee8021xRadiusServerEndpointConfiguration to populate with the parsed tokens.
 * @return true A valid Ieee8021xRadiusServerEndpointConfiguration was successfully parsed and written to the output parameter.
 * @return false No valid Ieee8021xRadiusServerEndpointConfiguration could be parsed from the input tokens.
 */
bool
ParseIeee8021xRadiusServerEndpointConfiguration(const std::vector<std::string>& ieee8021xRadiusServerEndpointConfigurationTokens, Ieee8021xRadiusServerEndpointConfiguration& ieee8021xRadiusServerEndpointConfiguration) noexcept
{
    uint16_t port{ 0 };
    std::string address{};
    std::string sharedSecret{};
    Ieee8021xRadiusServerEndpointType endpointType{ Ieee8021xRadiusServerEndpointType::Authentication };

    // Check the tokens positionally for required arguments. [[fallthrough]]s are used to allow optional fields to be
    // skipped, effectively shifting their positional index.
    for (std::size_t numArgument = 0; numArgument < std::size(ieee8021xRadiusServerEndpointConfigurationTokens); ++numArgument) {
        const auto& token = ieee8021xRadiusServerEndpointConfigurationTokens[numArgument];
        switch (numArgument) {
        // Parse the (optional) endpoint type. If not specified, defaults to 'Authentication'.
        case 0: {
            if (token == "acct" || token == "accounting") {
                ieee8021xRadiusServerEndpointConfiguration.Type = Ieee8021xRadiusServerEndpointType::Accounting;
                break;
            } else if (token == "auth" || token == "authentication") {
                ieee8021xRadiusServerEndpointConfiguration.Type = Ieee8021xRadiusServerEndpointType::Authentication;
                break;
            }
            // No recognized endpoint type specified. Fall through to the next case to check if the token contains the
            // 2nd expected field, an IP address.
            [[fallthrough]];
        }
        // Parse the ip address with optional port.
        case 1: {
            // Check if this token specifies an IP address.
            const auto ipAddressCategory = GetIpAddressCategory(token);
            if (ipAddressCategory != IpAddressCategory::Unknown) {
                // The address is valid; check if a port is specified.
                address = std::move(token);
                const auto portSeparatorIndex = address.find_last_of(':');
                if (portSeparatorIndex == std::string::npos) {
                    // The port is optional, so just take the address as-is.
                    break;
                }

                // Attempt to parse the port. While it is optional, if one is specified, it must be valid. So, return
                // false for all parsing errors.
                const auto portToken = address.substr(portSeparatorIndex + 1);
                try {
                    static constexpr auto PortMaximumValue{ std::numeric_limits<decltype(port)>::max() };

                    const auto portParsed = std::stoul(portToken);
                    if (portParsed > PortMaximumValue) {
                        LOGE << std::format("Port '{}' exceeds maximum value '{}' for RADIUS server endpoint configuration", portToken, PortMaximumValue);
                        return false;
                    }

                    // Port is valid.
                    port = static_cast<uint16_t>(portParsed);
                    address = address.substr(0, portSeparatorIndex);
                    break;
                } catch (const std::exception& e) {
                    LOGE << std::format("Failed to parse port '{}' for RADIUS server endpoint configuration: {}", portToken, e.what());
                    return false;
                }
            }
            // The token didn't contain an IP address. Fall through to the next case to process it as the shared secret.
            [[fallthrough]];
        }
        // Parse the shared secret.
        case 2: {
            sharedSecret = std::move(token);
            break;
        }
        default: {
            LOGW << std::format("Ignoring unexpected extra argument '{}' for RADIUS server endpoint configuration", token);
            break;
        }
        } // switch
    } // for

    // Ensure all required values were parsed.
    if (std::empty(address)) {
        LOGE << "Missing address for RADIUS server endpoint configuration";
        return false;
    }
    if (std::empty(sharedSecret)) {
        LOGE << "Missing shared secret for RADIUS server endpoint configuration";
        return false;
    }

    // Assign parsed values.
    ieee8021xRadiusServerEndpointConfiguration.Type = endpointType;
    ieee8021xRadiusServerEndpointConfiguration.Address = std::move(address);
    ieee8021xRadiusServerEndpointConfiguration.SharedSecret = { std::make_move_iterator(std::begin(sharedSecret)), std::make_move_iterator(std::end(sharedSecret)) };
    if (port != 0) {
        ieee8021xRadiusServerEndpointConfiguration.Port = port;
    }

    return true;
}

/**
 * @brief Helper that will be invoked by CLI11 when parsing a string argument for a RADIUS server endpoint configuration
 * of type Ieee8021xRadiusServerEndpointConfiguration.
 *
 * @param inputStream The input stream containing the arguments to parse a Ieee8021xRadiusServerEndpointConfiguration.
 * @param ieee8021xRadiusConfiguration The Ieee8021xRadiusServerEndpointConfiguration to populate with the parsed arguments.
 * @return std::istream&
 */
std::istream&
operator>>(std::istream& inputStream, Ieee8021xRadiusServerEndpointConfiguration& ieee8021xRadiusConfiguration)
{
    std::string token{};
    std::vector<std::string> tokens{};

    // Tokenize the stream by comma, the expected delimeter for arguments specifying a Ieee8021xRadiusServerEndpointConfiguration.
    while (std::getline(inputStream, token, ',')) {
        tokens.push_back(std::move(token));
    }

    // Attempt to parse the tokenized arguments into a Ieee8021xRadiusServerEndpointConfiguration. If successful, clear
    // the stream of the error/eof bits to signal to CLI11 that parsing was successful.
    const bool parsingSucceeded = ParseIeee8021xRadiusServerEndpointConfiguration(tokens, ieee8021xRadiusConfiguration);
    if (parsingSucceeded) {
        inputStream.clear();
    }

    return inputStream;
}
} // namespace Microsoft::Net

CLI::App*
NetRemoteCli::AddSubcommandWifiAccessPointSet8021xRadius(CLI::App* parent)
{
    auto* cliAppWifiAccessPointSetAuthenticationDot1x = parent->add_subcommand("access-point-set-radius", "Set the 802.1X RADIUS configuration for an access point");
    cliAppWifiAccessPointSetAuthenticationDot1x->alias("ap-set-radius")->alias("ap-radius")->alias("radius");
    cliAppWifiAccessPointSetAuthenticationDot1x->preparse_callback([this]([[maybe_unused]] std::size_t numArguments) {
        m_cliData->WifiAccessPointAuthentication8021x.Radius.emplace();
    });
    cliAppWifiAccessPointSetAuthenticationDot1x->callback([this] {
        Ieee8021xRadiusConfiguration* ieee8021xRadiusConfiguration{ nullptr };
        if (m_cliData->WifiAccessPointAuthentication8021x.Radius.has_value()) {
            ieee8021xRadiusConfiguration = &m_cliData->WifiAccessPointAuthentication8021x.Radius.value();

            // Force the primary server type(s) in case they were explicitly specified otherwise on the command line.
            ieee8021xRadiusConfiguration->AuthenticationServer.Type = Ieee8021xRadiusServerEndpointType::Authentication;
            if (ieee8021xRadiusConfiguration->AccountingServer.has_value()) {
                ieee8021xRadiusConfiguration->AccountingServer->Type = Ieee8021xRadiusServerEndpointType::Accounting;
            }

            LOGD << "Parsed 802.1X RADIUS configuration:\n"
                 << ToString(*ieee8021xRadiusConfiguration);
        }

        OnWifiAccessPointSet8021xRadius(m_cliData->WifiAccessPointId, ieee8021xRadiusConfiguration);
    });

    cliAppWifiAccessPointSetAuthenticationDot1x->add_option("id", m_cliData->WifiAccessPointId, "The identifier of the access point to set the 802.1X RADIUS configuration for")->required();
    cliAppWifiAccessPointSetAuthenticationDot1x->add_option("auth,--auth,--authServer,--authenticationServer", m_cliData->WifiAccessPointAuthentication8021x.Radius->AuthenticationServer, "The 802.1X RADIUS authentication server (format='<ip>[:<port>],<sharedsecret>')")->required();
    cliAppWifiAccessPointSetAuthenticationDot1x->add_option("--acct,--acctServer,--accountingServer", m_cliData->WifiAccessPointAuthentication8021x.Radius->AccountingServer, "The 802.1X RADIUS accounting server (format='<ip>[:<port>],<sharedsecret>')");
    cliAppWifiAccessPointSetAuthenticationDot1x->add_option("--fallback,--fallbackServer", m_cliData->WifiAccessPointAuthentication8021x.Radius->FallbackServers, "An 802.1X RADIUS authentication or accounting fallback server (format='[<[auth|acct]>,]<ip>[:<port>],<sharedsecret>')");

    return cliAppWifiAccessPointSetAuthenticationDot1x;
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
NetRemoteCli::OnNetworkInterfacesEnumerate()
{
    m_cliHandler->HandleCommandNetworkInterfacesEnumerate();
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

void
NetRemoteCli::OnWifiAccessPointSetSsid(std::string_view accessPointId, std::string_view ssid)
{
    m_cliHandler->HandleCommandWifiAccessPointSetSsid(accessPointId, ssid);
}

void
NetRemoteCli::OnWifiAccessPointSet8021xRadius(std::string_view accessPointId, const Ieee8021xRadiusConfiguration* ieee8021xRadiusConfiguration)
{
    m_cliHandler->HandleCommandWifiAccessPointSet8021xRadius(accessPointId, ieee8021xRadiusConfiguration);
}
