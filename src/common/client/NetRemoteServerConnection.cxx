
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <microsoft/net/remote/NetRemoteServerConnection.hxx>
#include <plog/Log.h>

using namespace Microsoft::Net::Remote;

using namespace Microsoft::Net::Remote::Service;
using namespace Microsoft::Net::Remote::Wifi;

NetRemoteServerConnection::NetRemoteServerConnection(std::string_view address, std::shared_ptr<grpc::Channel> channel, std::unique_ptr<NetRemote::Stub> client) :
    Address(address),
    Channel(channel),
    Client(std::move(client))
{
}

/* static */
std::shared_ptr<NetRemoteServerConnection>
NetRemoteServerConnection::TryEstablishConnection(std::string_view address)
{
    // Attempt to establish a connection (channel) to the server.
    static constexpr auto DontTryToConnect{ false };
    auto channel = grpc::CreateChannel(std::data(address), grpc::InsecureChannelCredentials());
    if (channel == nullptr || channel->GetState(DontTryToConnect) != GRPC_CHANNEL_IDLE) {
        LOGE << "Failed to establish connection channel";
        return nullptr;
    }

    // Attempt to create a new client API stub.
    auto client = NetRemote::NewStub(channel);
    if (client == nullptr) {
        LOGE << "Failed to create NetRemote API stub";
        return nullptr;
    }

    // Return a new connection object.
    auto netRemoteServerConnection = std::make_shared<NetRemoteServerConnection>(address, channel, std::move(client));
    return netRemoteServerConnection;
}
