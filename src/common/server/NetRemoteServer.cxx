
#include <format>
#include <memory>

#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server_builder.h>
#include <microsoft/net/remote/NetRemoteServer.hxx>
#include <microsoft/net/remote/NetRemoteServerConfiguration.hxx>
#include <microsoft/net/remote/NetRemoteService.hxx>
#include <plog/Log.h>

using namespace Microsoft::Net::Remote;

NetRemoteServer::NetRemoteServer(const NetRemoteServerConfiguration& configuration) :
    m_serverAddress(configuration.ServerAddress),
    m_service(configuration.AccessPointManager)
{}

NetRemoteServer::~NetRemoteServer()
{
    Stop();
}

std::unique_ptr<grpc::Server>&
NetRemoteServer::GetGrpcServer() noexcept
{
    return m_server;
}

Service::NetRemoteService&
NetRemoteServer::GetService() noexcept
{
    return m_service;
}

void
NetRemoteServer::Run()
{
    if (m_server != nullptr) {
        return;
    }

    grpc::ServerBuilder builder{};
    builder.AddListeningPort(m_serverAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(&m_service);

    m_server = builder.BuildAndStart();
    LOGI << std::format("Netremote server started listening on {}", m_serverAddress);
}

void
NetRemoteServer::Stop()
{
    if (m_server == nullptr) {
        return;
    }

    m_server->Shutdown();
    m_server = nullptr;
}
