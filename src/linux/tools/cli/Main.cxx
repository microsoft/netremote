
#include <format>

#include <magic_enum.hpp>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/MessageOnlyFormatter.h>
#include <plog/Init.h>
#include <plog/Log.h>

#include <grpcpp/create_channel.h>
#include <microsoft/net/remote/protocol/NetRemoteService.grpc.pb.h>

enum class NetRemoteServerCommand {
    None,
    WifiEnumerateAcessPoints,
};

// Default address to use if none specified on command-line.
constexpr auto RemoteServiceAddressHttpDefault = "localhost:5047";

/**
 * @brief Helper structure to contain all server connection information.
 */
struct NetRemoteServerConnection
{
    std::string Address{ RemoteServiceAddressHttpDefault };
    std::shared_ptr<grpc::Channel> Channel;
    std::unique_ptr<Microsoft::Net::Remote::Service::NetRemote::Stub> Client;
    grpc::ClientContext ClientContext;
};

using namespace Microsoft::Net::Remote::Service;
using namespace Microsoft::Net::Remote::Wifi;

/**
 * @brief Sends the WifiEnumerateAccessPoints request to the remote service.
 * 
 * @param connection The server connection to use to send the request.
 * @return int 
 */
int
SendWifiEnumerateAccessPointsRequest(NetRemoteServerConnection& connection)
{
    WifiEnumerateAccessPointsRequest request{};
    WifiEnumerateAccessPointsResult result{};

    auto status = connection.Client->WifiEnumerateAccessPoints(&connection.ClientContext, request, &result);
    if (!status.ok()) {
        LOGE << std::format("WifiEnumerateAccessPoints failed ({} {} {})", magic_enum::enum_name(status.error_code()), status.error_details(), status.error_message());
        return -1;
    }

    LOGI << std::format("WifiEnumerateAccessPoints succeeded, found {} access points", result.accesspoints_size());
    return 0;
}

int
main(int argc, char *argv[])
{
    // Set up basic console logging.
    static plog::ColorConsoleAppender<plog::MessageOnlyFormatter> colorConsoleAppender{};
    plog::init(plog::verbose, &colorConsoleAppender);

    NetRemoteServerConnection connection{};
    NetRemoteServerCommand commandToSend{ NetRemoteServerCommand::WifiEnumerateAcessPoints };

    // Determine remote service address to use.
    if (argc > 1) {
        connection.Address = argv[1];
    }

    // Attempt to establish a connection (channel) to the server.
    LOGI << std::format("Attempting to establish connection to {}", connection.Address);
    static constexpr auto DontTryToConnect{ false };
    connection.Channel = grpc::CreateChannel(connection.Address, grpc::InsecureChannelCredentials());
    if (connection.Channel == nullptr || connection.Channel->GetState(DontTryToConnect) != GRPC_CHANNEL_IDLE) {
        LOGE << "Failed to establish connection channel";
        return -1;
    }

    // Attempt to create a new API stub.
    connection.Client = NetRemote::NewStub(connection.Channel);
    if (connection.Client == nullptr) {
        LOGE << "Failed to create NetRemote API stub";
    }

    // TODO: Parse command-line arguments to determine command to send.
    // Send the requested command to the server.
    int ret;
    switch (commandToSend) {
    case NetRemoteServerCommand::WifiEnumerateAcessPoints:
        ret = SendWifiEnumerateAccessPointsRequest(connection);
        break;
    default:
        LOGW << "No command specified";
        ret = -1;
        break;
    }

    return ret;
}
