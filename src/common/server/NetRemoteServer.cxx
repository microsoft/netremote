
#include <format>
#include <memory>

#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server_builder.h>
#include <microsoft/net/remote/NetRemoteDiscoveryService.hxx>
#include <microsoft/net/remote/NetRemoteServer.hxx>
#include <microsoft/net/remote/NetRemoteServerConfiguration.hxx>
#include <microsoft/net/remote/NetRemoteService.hxx>
#include <plog/Log.h>

using namespace Microsoft::Net::Remote;

NetRemoteServer::NetRemoteServer(const NetRemoteServerConfiguration& configuration) :
    m_serverAddress(configuration.ServerAddress),
    m_service(configuration.AccessPointManager),
    m_discoveryServiceFactory(std::move(configuration.DiscoveryServiceFactory)),
    m_networkOperations(std::move(configuration.NetworkOperations))
{
    InitializeDiscoveryService();
}

NetRemoteServer::~NetRemoteServer()
{
    Stop();
}

void
NetRemoteServer::InitializeDiscoveryService()
{
    if (m_discoveryService != nullptr) {
        LOGW << "Discovery service is already initialized; skipping initialization";
        return;
    }
    if (m_networkOperations == nullptr) {
        LOGW << "Network operations is not available; server will not be discoverable";
        return;
    }
    if (m_discoveryServiceFactory == nullptr) {
        LOGW << "Discovery service factory is not available; server will not be discoverable";
        return;
    }

    NetRemoteDiscoveryServiceConfiguration discoveryServiceConfiguration{};
    discoveryServiceConfiguration.IpAddresses = m_networkOperations->GetLocalIpAddressInformation(m_serverAddress);
    auto discoveryService = m_discoveryServiceFactory->Create(std::move(discoveryServiceConfiguration));
    if (discoveryService == nullptr) {
        LOGE << "Failed to create discovery service; server will not be discoverable";
        return;
    }

    m_discoveryService = std::move(discoveryService);
    LOGI << "Discovery service initialized";
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

Service::NetRemoteDataStreamingService&
NetRemoteServer::GetDataStreamingService() noexcept
{
    return m_dataStreamingService;
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
    builder.RegisterService(&m_dataStreamingService);

    m_server = builder.BuildAndStart();
    LOGI << std::format("Netremote server started listening on {}", m_serverAddress);

    if (m_discoveryService != nullptr) {
        LOGI << "Starting discovery service";
        m_discoveryService->Start();
    } else {
        LOGW << "Discovery service not available; server will not be discoverable";
    }
}

void
NetRemoteServer::Stop()
{
    if (m_discoveryService != nullptr) {
        LOGI << "Stopping discovery service";
        m_discoveryService->Stop();
    }

    if (m_server == nullptr) {
        return;
    }
    m_server->Shutdown();
    m_server = nullptr;
}
