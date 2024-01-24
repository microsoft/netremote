
#include <format>
#include <sstream>

#include <grpcpp/client_context.h>
#include <magic_enum.hpp>
#include <microsoft/net/remote/NetRemoteCliHandlerOperations.hxx>
#include <microsoft/net/remote/protocol/NetRemoteService.grpc.pb.h>
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

    constexpr auto PhyTypePrefixLength = std::size(std::string_view("Dot11PhyType"));
    ss << indent0
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
    for (const auto& band : accessPointCapabilities.bands()) {
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
    WifiEnumerateAccessPointsRequest request{};
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

std::unique_ptr<INetRemoteCliHandlerOperations>
NetRemoteCliHandlerOperationsFactory::Create(std::shared_ptr<NetRemoteServerConnection> connection)
{
    return std::make_unique<NetRemoteCliHandlerOperations>(std::move(connection));
}
