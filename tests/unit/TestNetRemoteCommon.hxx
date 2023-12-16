
#ifndef TEST_NET_REMOTE_COMMON_HXX
#define TEST_NET_REMOTE_COMMON_HXX

#include <algorithm>
#include <chrono>
#include <memory>
#include <ranges>
#include <string_view>
#include <tuple>
#include <vector>

#include <microsoft/net/remote/protocol/NetRemoteService.grpc.pb.h>

namespace Microsoft::Net::Remote::Test
{
using namespace std::chrono_literals;

/**
 * @brief Default server address (insecure).
 */
constexpr auto RemoteServiceAddressHttp = "localhost:5047";

/**
 * @brief Default client connection timeout.
 */
constexpr auto RemoteServiceConnectionTimeout = 3s;

/**
 * @brief Establish the specified number of connections to a netremote server
 * with specified address and default credentials.
 *
 * @param numConnectionsToEstablish The number of client connections to establish.
 * @param serverAddress The server address to connect to. Defaults to RemoteServiceAddressHttp.
 * @return std::vector<std::tuple<std::shared_ptr<grpc::Channel>, std::unique_ptr<Microsoft::Net::Remote::Service::NetRemote::Stub>>>
 */
std::vector<std::tuple<std::shared_ptr<grpc::Channel>, std::unique_ptr<Microsoft::Net::Remote::Service::NetRemote::Stub>>>
EstablishClientConnections(std::size_t numConnectionsToEstablish, std::string_view serverAddress = RemoteServiceAddressHttp);

} // namespace Microsoft::Net::Remote::Test

#endif // TEST_NET_REMOTE_COMMON_HXX
