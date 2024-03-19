
#include <cstddef>
#include <format>
#include <iterator>
#include <memory>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include <grpcpp/client_context.h>
#include <magic_enum.hpp>
#include <microsoft/net/remote/INetRemoteCliHandlerOperations.hxx>
#include <microsoft/net/remote/NetRemoteCliHandlerOperations.hxx>
#include <microsoft/net/remote/NetRemoteServerConnection.hxx>
#include <microsoft/net/remote/protocol/NetRemoteService.grpc.pb.h>
#include <microsoft/net/remote/protocol/NetRemoteWifi.pb.h>
#include <microsoft/net/remote/protocol/WifiCore.pb.h>
#include <microsoft/net/wifi/Ieee80211Dot11Adapters.hxx>
#include <plog/Log.h>

using namespace Microsoft::Net::Remote;
using namespace Microsoft::Net::Remote::Service;
using namespace Microsoft::Net::Remote::Wifi;
using namespace Microsoft::Net::Wifi;

NetRemoteCliHandlerOperations::NetRemoteCliHandlerOperations(std::shared_ptr<NetRemoteServerConnection> connection) :
    m_connection(std::move(connection))
{
}

namespace detail
{
enum PrintOption {
    None,
    WithSuffix,
};

/**
 * @brief Convert a Dot11FrequencyBand to a string representation.
 *
 * @tparam printOption Option to control the output format.
 * @param frequencyBand The frequency band to convert.
 * @return std::string_view
 */
template <PrintOption printOption = PrintOption::None>
constexpr std::string_view
Dot11FrequencyBandToString(Dot11FrequencyBand frequencyBand)
{
#define TWO_POINT_4  "2.4"
#define FIVE_POINT_0 "5.0"
#define SIX_POINT_0  "6.0"
#define GHZ          " GHz"
#define QQ           "??"

    if constexpr (printOption == PrintOption::WithSuffix) {
        switch (frequencyBand) {
        case Dot11FrequencyBand::Dot11FrequencyBand2_4GHz:
            return TWO_POINT_4 GHZ;
        case Dot11FrequencyBand::Dot11FrequencyBand5_0GHz:
            return FIVE_POINT_0 GHZ;
        case Dot11FrequencyBand::Dot11FrequencyBand6_0GHz:
            return SIX_POINT_0 GHZ;
        default:
            return QQ;
        }
    } else {
        switch (frequencyBand) {
        case Dot11FrequencyBand::Dot11FrequencyBand2_4GHz:
            return TWO_POINT_4;
        case Dot11FrequencyBand::Dot11FrequencyBand5_0GHz:
            return FIVE_POINT_0;
        case Dot11FrequencyBand::Dot11FrequencyBand6_0GHz:
            return SIX_POINT_0;
        default:
            return QQ;
        }
    }

#undef TWO_POINT_4
#undef FIVE_POINT_0
#undef SIX_POINT_0
#undef GHZ
#undef QQ
}

/**
 * @brief Convert a Dot11SecurityProtocol to a string representation.
 *
 * @param securityProtocol The security protocol to convert.
 * @return constexpr std::string_view
 */
constexpr std::string_view
Dot11SecurityProtocolToString(Dot11SecurityProtocol securityProtocol)
{
    switch (securityProtocol) {
    case Dot11SecurityProtocol::Dot11SecurityProtocolWpa1:
        return "WPA1";
    case Dot11SecurityProtocol::Dot11SecurityProtocolWpa2:
        return "WPA2";
    case Dot11SecurityProtocol::Dot11SecurityProtocolWpa3:
        return "WPA3";
    default:
        return "??";
    }
}

constexpr auto PhyTypePrefixLength = std::size(std::string_view("Dot11PhyType"));
constexpr auto AkmSuitePrefixLength = std::size(std::string_view("Dot11AkmSuite"));
constexpr auto BandPrefixLength = std::size(std::string_view("Dot11FrequencyBand"));
constexpr auto CipherAlgorithmPrefixLength = std::size(std::string_view("Dot11CipherSuite"));

/**
 * @brief Convert a Dot11AccessPointCapabilities to a detailed string representation.
 *
 * @param accessPointCapabilities The capabilities to convert.
 * @param indent0 The first level of indentation.
 * @param indent1 The second level of indentation.
 * @return std::string
 */
std::string
NetRemoteAccessPointCapabilitiesToStringDetailed(const Dot11AccessPointCapabilities& accessPointCapabilities, std::string_view indent0, std::string_view indent1)
{
    std::stringstream ss;

    ss << indent0
       << "Security Protocols: ";
    for (const auto& securityProtocol : accessPointCapabilities.securityprotocols()) {
        if (securityProtocol != accessPointCapabilities.securityprotocols()[0]) {
            ss << ' ';
        }
        ss << Dot11SecurityProtocolToString(static_cast<Dot11SecurityProtocol>(securityProtocol));
    }

    ss << '\n'
       << indent0
       << "Phy Types: 802.11 ";
    for (const auto& phyType : accessPointCapabilities.phytypes()) {
        std::string_view phyTypeName(magic_enum::enum_name(static_cast<Dot11PhyType>(phyType)));
        phyTypeName.remove_prefix(PhyTypePrefixLength);
        if (phyType != accessPointCapabilities.phytypes()[0]) {
            ss << ' ';
        }
        ss << phyTypeName;
    }

    ss << '\n'
       << indent0
       << "Frequency Bands:";
    for (const auto& band : accessPointCapabilities.frequencybands()) {
        std::string_view bandName(magic_enum::enum_name(static_cast<Dot11FrequencyBand>(band)));
        bandName.remove_prefix(BandPrefixLength);
        ss << '\n'
           << indent1 << bandName;
    }

    ss << '\n'
       << indent0
       << "Authentication and Key Management (AKM) Suites:";
    for (const auto& akmSuite : accessPointCapabilities.akmsuites()) {
        std::string_view akmSuiteName(magic_enum::enum_name(static_cast<Dot11AkmSuite>(akmSuite)));
        akmSuiteName.remove_prefix(AkmSuitePrefixLength);
        ss << '\n'
           << indent1 << akmSuiteName;
    }

    ss << '\n'
       << indent0
       << "Cipher Algorithms:";
    for (const auto& ciperSuite : accessPointCapabilities.ciphersuites()) {
        std::string_view cipherSuiteName(magic_enum::enum_name(static_cast<Dot11CipherSuite>(ciperSuite)));
        cipherSuiteName.remove_prefix(CipherAlgorithmPrefixLength);
        ss << '\n'
           << indent1 << cipherSuiteName;
    }

    return ss.str();
}

/**
 * @brief Convert a Dot11AccessPointCapabilities to a brief, single-line string representation.
 *
 * @param accessPointCapabilities The capabilities to convert.
 * @param indent The indentation to use.
 * @return std::string
 */
std::string
NetRemoteAccessPointCapabilitiesToStringBrief(const Dot11AccessPointCapabilities& accessPointCapabilities)
{
    std::stringstream ss;

    for (const auto& securityProtocol : accessPointCapabilities.securityprotocols()) {
        if (securityProtocol != accessPointCapabilities.securityprotocols()[0]) {
            ss << ' ';
        }
        ss << Dot11SecurityProtocolToString(static_cast<Dot11SecurityProtocol>(securityProtocol));
    }

    ss << " IEEE 802.11 ";
    for (const auto& phyType : accessPointCapabilities.phytypes()) {
        std::string_view phyTypeName(magic_enum::enum_name(static_cast<Dot11PhyType>(phyType)));
        phyTypeName.remove_prefix(PhyTypePrefixLength);
        if (phyType != accessPointCapabilities.phytypes()[0]) {
            ss << '/';
        }
        ss << phyTypeName;
    }

    auto frequencyBands = accessPointCapabilities.frequencybands();
    std::ranges::sort(frequencyBands);
    ss << ' ';
    for (const auto& frequencyBand : frequencyBands) {
        if (frequencyBand != frequencyBands[0]) {
            ss << ' ';
        }
        ss << Dot11FrequencyBandToString<PrintOption::WithSuffix>(static_cast<Dot11FrequencyBand>(frequencyBand));
    }

    return ss.str();
}

/**
 * @brief Generate a string representation of a Dot11AccessPointCapabilities object.
 *
 * @param accessPointCapabilities The object to generate a string representation of.
 * @param detailedOutput Whether the output should be detailed (true) or brief (false, default).
 * @param indentationLevel The indentation level to use.
 * @param indentation The number of spaces in each indentation level.
 * @return std::string
 */
std::string
NetRemoteAccessPointCapabilitiesToString(const Dot11AccessPointCapabilities& accessPointCapabilities, bool detailedOutput = false, std::size_t indentationLevel = 1, std::size_t indentation = 2)
{
    if (!detailedOutput) {
        return NetRemoteAccessPointCapabilitiesToStringBrief(accessPointCapabilities);
    } else {
        const auto indent0 = std::string((indentationLevel + 0) * indentation, ' ');
        const auto indent1 = std::string((indentationLevel + 1) * indentation, ' ');

        return NetRemoteAccessPointCapabilitiesToStringDetailed(accessPointCapabilities, indent0, indent1);
    }
}
} // namespace detail

void
NetRemoteCliHandlerOperations::WifiEnumerateAccessPoints(bool detailedOutput)
{
    const WifiEnumerateAccessPointsRequest request{};
    WifiEnumerateAccessPointsResult result{};
    grpc::ClientContext clientContext{};

    auto status = m_connection->Client->WifiEnumerateAccessPoints(&clientContext, request, &result);
    if (!status.ok()) {
        LOGE << std::format("Failed to enumerate WiFi access points, error={} details={} message={}", magic_enum::enum_name(status.error_code()), status.error_details(), status.error_message());
        return;
    }

    if (result.accesspoints().empty()) {
        LOGI << "No access points found";
        return;
    }

    std::size_t numAccessPoint = 1;
    for (const auto& accessPoint : result.accesspoints()) {
        std::stringstream ss;
        ss << '[' << numAccessPoint++ << "] " << accessPoint.accesspointid();
        if (!accessPoint.isenabled()) {
            ss << " (disabled)";
        }
        ss << ((detailedOutput) ? "\n" : ": ")
           << detail::NetRemoteAccessPointCapabilitiesToString(accessPoint.capabilities(), detailedOutput);
        LOGI << ss.str();
    }
}

void
NetRemoteCliHandlerOperations::WifiAccessPointEnable(std::string_view accessPointId, const Ieee80211AccessPointConfiguration* ieee80211AccessPointConfiguration)
{
    WifiAccessPointEnableRequest request{};
    WifiAccessPointEnableResult result{};
    grpc::ClientContext clientContext{};

    request.set_accesspointid(std::string(accessPointId));

    // Populate access point configuration if present.
    if (ieee80211AccessPointConfiguration != nullptr) {
        auto& dot11AccessPointConfiguration = *request.mutable_configuration();

        // Populate SSID if present.
        if (ieee80211AccessPointConfiguration->Ssid.has_value()) {
            dot11AccessPointConfiguration.mutable_ssid()->set_name(ieee80211AccessPointConfiguration->Ssid.value());
        }

        // Populate PHY type if present.
        if (ieee80211AccessPointConfiguration->PhyType.has_value()) {
            const auto dot11PhyType = ToDot11PhyType(ieee80211AccessPointConfiguration->PhyType.value());
            dot11AccessPointConfiguration.set_phytype(dot11PhyType);
        }

        // Populate pairwise cipher suites if present.
        if (!std::empty(ieee80211AccessPointConfiguration->PairwiseCipherSuites)) {
            auto dot11PairwiseCipherSuites = ToDot11CipherSuiteConfigurations(ieee80211AccessPointConfiguration->PairwiseCipherSuites);
            *dot11AccessPointConfiguration.mutable_pairwiseciphersuites() = {
                std::make_move_iterator(std::begin(dot11PairwiseCipherSuites)),
                std::make_move_iterator(std::end(dot11PairwiseCipherSuites))
            };
        }

        // Populate authentication algorithms if present.
        if (!std::empty(ieee80211AccessPointConfiguration->AuthenticationAlgorithms)) {
            auto dot11AuthenticationAlgorithms = ToDot11AuthenticationAlgorithms(ieee80211AccessPointConfiguration->AuthenticationAlgorithms);
            *dot11AccessPointConfiguration.mutable_authenticationalgorithms() = {
                std::make_move_iterator(std::begin(dot11AuthenticationAlgorithms)),
                std::make_move_iterator(std::end(dot11AuthenticationAlgorithms))
            };
        }

        // Populate frequency bands if present.
        if (!std::empty(ieee80211AccessPointConfiguration->FrequencyBands)) {
            auto dot11FrequencyBands = ToDot11FrequencyBands(ieee80211AccessPointConfiguration->FrequencyBands);
            *dot11AccessPointConfiguration.mutable_frequencybands() = {
                std::make_move_iterator(std::begin(dot11FrequencyBands)),
                std::make_move_iterator(std::end(dot11FrequencyBands))
            };
        }
    }

    auto status = m_connection->Client->WifiAccessPointEnable(&clientContext, request, &result);
    if (!status.ok()) {
        LOGE << std::format("Failed to enable WiFi access point, error={} details={} message={}", magic_enum::enum_name(status.error_code()), status.error_details(), status.error_message());
        return;
    }
}

void
NetRemoteCliHandlerOperations::WifiAccessPointDisable(std::string_view accessPointId)
{
    WifiAccessPointDisableRequest request{};
    WifiAccessPointDisableResult result{};
    grpc::ClientContext clientContext{};

    request.set_accesspointid(std::string(accessPointId));

    auto status = m_connection->Client->WifiAccessPointDisable(&clientContext, request, &result);
    if (!status.ok()) {
        LOGE << std::format("Failed to disable WiFi access point, error={} details={} message={}", magic_enum::enum_name(status.error_code()), status.error_details(), status.error_message());
        return;
    }
}

std::unique_ptr<INetRemoteCliHandlerOperations>
NetRemoteCliHandlerOperationsFactory::Create(std::shared_ptr<NetRemoteServerConnection> connection)
{
    return std::make_unique<NetRemoteCliHandlerOperations>(std::move(connection));
}
