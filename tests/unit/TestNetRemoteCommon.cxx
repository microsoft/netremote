
#include <catch2/catch_test_macros.hpp>
#include <grpcpp/create_channel.h>

#include "TestNetRemoteCommon.hxx"

using namespace Microsoft::Net::Remote;
using namespace Microsoft::Net::Remote::Test;
using namespace Microsoft::Net::Remote::Service;

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
