
#include <format>
#include <iostream>
#include <stdexcept>

#include <grpcpp/server_builder.h>
#include <microsoft/net/remote/NetRemoteServer.hxx>

using namespace Microsoft::Net::Remote;

NetRemoteServer::NetRemoteServer(std::string_view serverAddress) :
    m_serverAddress{ serverAddress }
{}

std::unique_ptr<grpc::Server>& NetRemoteServer::GetGrpcServer() noexcept
{
    return m_server;
}

void NetRemoteServer::Run()
{
    if (m_server != nullptr)
    {
        return;
    }

    grpc::ServerBuilder builder{};
    builder.AddListeningPort(m_serverAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(&m_service);

    m_server = builder.BuildAndStart();
    std::cout << std::format("Started listening on {}", m_serverAddress) << std::endl;
}

void NetRemoteServer::Stop()
{
    if (m_server == nullptr)
    {
        return;
    }

    m_server->Shutdown();
    m_server = nullptr;
}
