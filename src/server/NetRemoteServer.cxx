
#include <format>
#include <iostream>
#include <stdexcept>

#include <grpcpp/server_builder.h>
#include <microsoft/net/remote/NetRemoteServer.hxx>

using namespace Microsoft::Net::Remote;

NetRemoteServer::NetRemoteServer(int argc, char* argv[])
{
    try 
    {
        m_cliParser->parse(argc, argv);
    }
    catch (const CLI::ParseError& e)
    {
        const auto failureMessage = std::format("Failed to parse command-line arguments ({}).", e.what());
        std::cerr << std::format("{}\n{}", failureMessage, m_cliParser->help()) << std::endl;
        throw std::runtime_error{ failureMessage };
    }

    // At this point, all supported command line arguments have been parsed.
    // However, there are no command-line arguments currently supported, so
    // there is nothing to do here. This should be updated to tease out the
    // command line arguments from m_cliParser and save them to member variables
    // if/when some arguments are supported.
}

/* static */
std::unique_ptr<CLI::App> NetRemoteServer::CreateCliParser()
{
    auto app = std::make_unique<CLI::App>("NetRemoteServer");
    // TODO: configure CLI11 app with supported options. Currently none.
    return app;
}

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
