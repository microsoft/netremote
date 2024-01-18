
#ifndef NET_REMOTE_SERVER_CONNECTION
#define NET_REMOTE_SERVER_CONNECTION

#include <memory>
#include <string>
#include <string_view>

#include <grpcpp/channel.h>
#include <microsoft/net/remote/protocol/NetRemoteService.grpc.pb.h>

namespace Microsoft::Net::Remote
{
/**
 * @brief Helper structure to collect server connection information.
 */
struct NetRemoteServerConnection
{
    std::string Address;
    std::shared_ptr<grpc::Channel> Channel;
    std::unique_ptr<Microsoft::Net::Remote::Service::NetRemote::Stub> Client;

    /**
     * @brief Construct a new NetRemoteServerConnection object with the specified address, channel, and client.
     *
     * @param address The address that was used to connect.
     * @param channel The established channel.
     * @param client The client stub that was obtained from 'channel'.
     */
    NetRemoteServerConnection(std::string_view address, std::shared_ptr<grpc::Channel> channel, std::unique_ptr<Microsoft::Net::Remote::Service::NetRemote::Stub> client);

    /**
     * @brief Attempt to establish a basic connection to a remote server.
     *
     * @param address The address to use to connect. Must be in the form <host>:<port>.
     * @return std::shared_ptr<NetRemoteServerConnection> 
     */
    static std::shared_ptr<NetRemoteServerConnection>
    TryEstablishConnection(std::string_view address);

    NetRemoteServerConnection(const NetRemoteServerConnection& other) = delete;
    NetRemoteServerConnection(NetRemoteServerConnection&& other) = default;

    NetRemoteServerConnection
    operator=(const NetRemoteServerConnection& other) = delete;

    NetRemoteServerConnection&
    operator=(NetRemoteServerConnection&& other) = default;
};
} // namespace Microsoft::Net::Remote

#endif // NET_REMOTE_SERVER_CONNECTION
