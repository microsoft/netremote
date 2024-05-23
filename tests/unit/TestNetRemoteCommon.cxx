
#include <catch2/catch_test_macros.hpp>
#include <grpcpp/create_channel.h>
#include <microsoft/net/NetworkOperationsLinux.hxx>
#include <microsoft/net/remote/service/NetRemoteServerConfiguration.hxx>
#include <microsoft/net/wifi/test/AccessPointManagerTest.hxx>

#include "TestNetRemoteCommon.hxx"

using namespace Microsoft::Net::Remote;
using namespace Microsoft::Net::Remote::Test;
using namespace Microsoft::Net::Remote::Service;
using namespace Microsoft::Net::Wifi;
using namespace Microsoft::Net::Wifi::Test;

std::vector<std::tuple<std::shared_ptr<grpc::Channel>, std::unique_ptr<NetRemote::Stub>>>
Microsoft::Net::Remote::Test::EstablishClientConnections(std::size_t numConnectionsToEstablish, std::string_view serverAddress)
{
    std::vector<std::tuple<std::shared_ptr<grpc::Channel>, std::unique_ptr<NetRemote::Stub>>> clients(numConnectionsToEstablish);
    std::ranges::transform(clients, std::begin(clients), [&](auto &&) {
        auto channel = grpc::CreateChannel(std::data(serverAddress), grpc::InsecureChannelCredentials());
        auto client = NetRemote::NewStub(channel);
        REQUIRE(channel->WaitForConnected(std::chrono::system_clock::now() + RemoteServiceConnectionTimeout));

        return std::make_tuple(std::move(channel), std::move(client));
    });

    return clients;
}

NetRemoteServerConfiguration
Microsoft::Net::Remote::Test::CreateServerConfiguration(std::shared_ptr<AccessPointManager> accessPointManager, std::shared_ptr<NetworkManager> networkManager)
{
    if (accessPointManager == nullptr) {
        accessPointManager = std::make_shared<AccessPointManagerTest>();
    }
    if (networkManager == nullptr) {
        networkManager = std::make_shared<NetworkManager>(std::make_unique<NetworkOperationsLinux>(), accessPointManager);
    }

    return {
        .ServerAddress = RemoteServiceAddressHttp,
        .NetworkManager = networkManager,
    };
}
