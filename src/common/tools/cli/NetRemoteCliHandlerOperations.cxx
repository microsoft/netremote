
#include <cstddef>
#include <format>
#include <iterator>
#include <memory>
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

NetRemoteCliHandlerOperations::NetRemoteCliHandlerOperations(std::shared_ptr<NetRemoteServerConnection> connection) :
    m_connection(std::move(connection))
{
}

namespace detail
{
/**
 * @brief Generate a string representation of a Dot11AccessPointCapabilities object.
 *
 * @param accessPointCapabilities The object to generate a string representation of.
 * @param indentationLevel The indentation level to use.
 * @param indentation The number of spaces in each indentation level.
 * @return std::string
 */
std::string
NetRemoteAccessPointCapabilitiesToString(const Microsoft::Net::Wifi::Dot11AccessPointCapabilities& accessPointCapabilities, std::size_t indentationLevel = 1, std::size_t indentation = 2)
{
    const auto indent0 = std::string((indentationLevel + 0) * indentation, ' ');
    const auto indent1 = std::string((indentationLevel + 1) * indentation, ' ');

    std::stringstream ss;

    constexpr auto SecurityProtocolPrefixLength = std::size(std::string_view("Dot11SecurityProtocol"));
    ss << indent0
       << "Security Protocols: ";
    for (const auto& securityProtocol : accessPointCapabilities.securityprotocols()) {
        std::string_view securityProtocolName(magic_enum::enum_name(static_cast<Microsoft::Net::Wifi::Dot11SecurityProtocol>(securityProtocol)));
        securityProtocolName.remove_prefix(SecurityProtocolPrefixLength);
        ss << '\n'
           << indent1 << securityProtocolName;
    }

    constexpr auto PhyTypePrefixLength = std::size(std::string_view("Dot11PhyType"));
    ss << '\n'
       << indent0
       << "Phy Types: ";
    for (const auto& phyType : accessPointCapabilities.phytypes()) {
        std::string_view phyTypeName(magic_enum::enum_name(static_cast<Microsoft::Net::Wifi::Dot11PhyType>(phyType)));
        phyTypeName.remove_prefix(PhyTypePrefixLength);
        if (phyType != accessPointCapabilities.phytypes()[0]) {
            ss << ' ';
        }
        ss << phyTypeName;
    }

    constexpr auto BandPrefixLength = std::size(std::string_view("Dot11FrequencyBand"));
    ss << '\n'
       << indent0
       << "Bands:";
    for (const auto& band : accessPointCapabilities.frequencybands()) {
        std::string_view bandName(magic_enum::enum_name(static_cast<Microsoft::Net::Wifi::Dot11FrequencyBand>(band)));
        bandName.remove_prefix(BandPrefixLength);
        ss << '\n'
           << indent1 << bandName;
    }

    constexpr auto AkmSuitePrefixLength = std::size(std::string_view("Dot11AkmSuite"));
    ss << '\n'
       << indent0
       << "Authentication and Key Management (AKM) Suites:";
    for (const auto& akmSuite : accessPointCapabilities.akmsuites()) {
        std::string_view akmSuiteName(magic_enum::enum_name(static_cast<Microsoft::Net::Wifi::Dot11AkmSuite>(akmSuite)));
        akmSuiteName.remove_prefix(AkmSuitePrefixLength);
        ss << '\n'
           << indent1 << akmSuiteName;
    }

    constexpr auto CipherAlgorithmPrefixLength = std::size(std::string_view("Dot11CipherSuite"));
    ss << '\n'
       << indent0
       << "Cipher Algorithms:";
    for (const auto& ciperSuite : accessPointCapabilities.ciphersuites()) {
        std::string_view cipherSuiteName(magic_enum::enum_name(static_cast<Microsoft::Net::Wifi::Dot11CipherSuite>(ciperSuite)));
        cipherSuiteName.remove_prefix(CipherAlgorithmPrefixLength);
        ss << '\n'
           << indent1 << cipherSuiteName;
    }

    return ss.str();
}
} // namespace detail

void
NetRemoteCliHandlerOperations::WifiEnumerateAccessPoints()
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
        ss << '\n'
           << detail::NetRemoteAccessPointCapabilitiesToString(accessPoint.capabilities())
           << '\n';
        LOGI << ss.str();
    }
}

void
NetRemoteCliHandlerOperations::WifiAccessPointEnable(std::string_view accessPointId, const std::optional<Microsoft::Net::Wifi::Ieee80211AccessPointConfiguration>& ieee80211AccessPointConfiguration)
{
    WifiAccessPointEnableRequest request{};
    WifiAccessPointEnableResult result{};
    grpc::ClientContext clientContext{};

    request.set_accesspointid(std::string(accessPointId));

    // Populate access point configuration if present.
    if (ieee80211AccessPointConfiguration.has_value()) {
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
