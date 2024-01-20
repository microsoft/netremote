#include <format>

#include <grpcpp/client_context.h>
#include <magic_enum.hpp>
#include <microsoft/net/remote/NetRemoteCliHandlerOperations.hxx>
#include <microsoft/net/remote/protocol/NetRemoteService.grpc.pb.h>
#include <plog/Log.h>

using namespace Microsoft::Net::Remote;

using namespace Microsoft::Net::Remote;
using namespace Microsoft::Net::Remote::Service;
using namespace Microsoft::Net::Remote::Wifi;

NetRemoteCliHandlerOperations::NetRemoteCliHandlerOperations(std::shared_ptr<NetRemoteServerConnection> connection) :
    m_connection(std::move(connection))
{
}

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

    LOGI << std::format("{} access points discovered", result.accesspoints_size());

    for (const auto& accessPoint : result.accesspoints()) {
        LOGI << std::format(" - [{}]", accessPoint.accesspointid());
        LOGI << std::format("   - {}", accessPoint.isenabled() ? "enabled" : "disabled");
    }
}

std::unique_ptr<INetRemoteCliHandlerOperations>
NetRemoteCliHandlerOperationsFactory::Create(std::shared_ptr<NetRemoteServerConnection> connection)
{
    return std::make_unique<NetRemoteCliHandlerOperations>(std::move(connection));
}
