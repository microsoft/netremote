
#include <format>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

#include <catch2/catch_test_macros.hpp>
#include <grpcpp/create_channel.h>
#include <magic_enum.hpp>
#include <microsoft/net/remote/NetRemoteServer.hxx>
#include <microsoft/net/remote/protocol/NetRemoteService.grpc.pb.h>
#include <microsoft/net/wifi/AccessPointManager.hxx>
#include <microsoft/net/wifi/test/AccessPointManagerTest.hxx>
#include <microsoft/net/wifi/test/AccessPointTest.hxx>

#include "TestNetRemoteCommon.hxx"

using Microsoft::Net::Remote::Test::RemoteServiceAddressHttp;
using Microsoft::Net::Wifi::Test::AccessPointFactoryTest;

TEST_CASE("WifiEnumerateAccessPoints API", "[basic][rpc][client][remote]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Remote::Wifi;
    using namespace Microsoft::Net::Wifi;

    using Microsoft::Net::Remote::Test::RemoteServiceAddressHttp;

    NetRemoteServerConfiguration Configuration{
        .ServerAddress = RemoteServiceAddressHttp,
        .AccessPointManager = AccessPointManager::Create(),
    };

    NetRemoteServer server{ Configuration };
    server.Run();

    auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
    auto client = NetRemote::NewStub(channel);

    SECTION("Can be called")
    {
        WifiEnumerateAccessPointsRequest request{};

        WifiEnumerateAccessPointsResult result{};
        grpc::ClientContext clientContext{};

        auto status = client->WifiEnumerateAccessPoints(&clientContext, request, &result);
        REQUIRE(status.ok());
        REQUIRE_NOTHROW([&] {
            [[maybe_unused]] const auto& accessPoints = result.accesspoints();
        });
    }

    SECTION("Initial enablement status is disabled")
    {
        WifiEnumerateAccessPointsRequest request{};

        WifiEnumerateAccessPointsResult result{};
        grpc::ClientContext clientContext{};

        auto status = client->WifiEnumerateAccessPoints(&clientContext, request, &result);
        REQUIRE(status.ok());

        for (const auto& accessPoint : result.accesspoints()) {
            REQUIRE(!accessPoint.isenabled());
        }
    }
}

TEST_CASE("WifiAccessPointEnable API", "[basic][rpc][client][remote]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Remote::Wifi;
    using namespace Microsoft::Net::Wifi;

    constexpr auto SsidName{ "TestWifiAccessPointEnable" };

    NetRemoteServerConfiguration Configuration{
        .ServerAddress = RemoteServiceAddressHttp,
        .AccessPointManager = AccessPointManager::Create(),
    };

    NetRemoteServer server{ Configuration };
    server.Run();

    auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
    auto client = NetRemote::NewStub(channel);

    SECTION("Can be called")
    {
        Dot11AccessPointConfiguration apConfiguration{};
        apConfiguration.mutable_ssid()->set_name(SsidName);
        apConfiguration.set_phytype(Dot11PhyType::Dot11PhyTypeA);
        apConfiguration.set_authenticationalgorithm(Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmSharedKey);
        apConfiguration.set_ciphersuite(Dot11CipherSuite::Dot11CipherSuiteCcmp256);
        apConfiguration.mutable_bands()->Add(Dot11FrequencyBand::Dot11FrequencyBand2_4GHz);
        apConfiguration.mutable_bands()->Add(Dot11FrequencyBand::Dot11FrequencyBand5_0GHz);

        WifiAccessPointEnableRequest request{};
        request.set_accesspointid("TestWifiAccessPointEnable");
        *request.mutable_configuration() = std::move(apConfiguration);

        WifiAccessPointEnableResult result{};
        grpc::ClientContext clientContext{};

        auto status = client->WifiAccessPointEnable(&clientContext, request, &result);
        REQUIRE(status.ok());
        REQUIRE(result.accesspointid() == request.accesspointid());
        REQUIRE(result.has_status());
        REQUIRE(result.status().code() == WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);
        REQUIRE(result.status().message().empty());
        REQUIRE(result.status().has_details() == false);
    }
}

TEST_CASE("WifiAccessPointSetPhyType API", "[basic][rpc][client][remote]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Remote::Wifi;
    using namespace Microsoft::Net::Wifi;
    using namespace Microsoft::Net::Wifi::Test;

    constexpr auto SsidName{ "TestWifiAccessPointSetPhyType" };
    constexpr auto InterfaceName{ "TestWifiAccessPointSetPhyType" };

    auto apManagerTest = std::make_shared<AccessPointManagerTest>();
    Ieee80211AccessPointCapabilities apCapabilities{
        .Protocols{ Ieee80211Protocol::B, Ieee80211Protocol::G, Ieee80211Protocol::N, Ieee80211Protocol::A, Ieee80211Protocol::AC, Ieee80211Protocol::AD, Ieee80211Protocol::AX, Ieee80211Protocol::AX }
    };
    auto apTest = std::make_shared<AccessPointTest>(InterfaceName, std::move(apCapabilities));
    apManagerTest->AddAccessPoint(apTest);

    NetRemoteServerConfiguration Configuration{
        .ServerAddress = RemoteServiceAddressHttp,
        .AccessPointManager = apManagerTest,
    };

    NetRemoteServer server{ Configuration };
    server.Run();

    auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
    auto client = NetRemote::NewStub(channel);

    Dot11AccessPointConfiguration apConfiguration{};
    apConfiguration.mutable_ssid()->set_name(SsidName);
    apConfiguration.set_phytype(Dot11PhyType::Dot11PhyTypeA);
    apConfiguration.set_authenticationalgorithm(Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmSharedKey);
    apConfiguration.set_ciphersuite(Dot11CipherSuite::Dot11CipherSuiteCcmp256);
    apConfiguration.mutable_bands()->Add(Dot11FrequencyBand::Dot11FrequencyBand2_4GHz);
    apConfiguration.mutable_bands()->Add(Dot11FrequencyBand::Dot11FrequencyBand5_0GHz);

    WifiAccessPointEnableRequest request{};
    request.set_accesspointid(InterfaceName);
    *request.mutable_configuration() = std::move(apConfiguration);

    WifiAccessPointEnableResult result{};
    grpc::ClientContext clientContext{};

    auto status = client->WifiAccessPointEnable(&clientContext, request, &result);
    REQUIRE(status.ok());
    REQUIRE(result.accesspointid() == request.accesspointid());
    REQUIRE(result.has_status());
    REQUIRE(result.status().code() == WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);
    REQUIRE(result.status().message().empty());
    REQUIRE(result.status().has_details() == false);

    SECTION("Can be called")
    {
        WifiAccessPointSetPhyTypeRequest setPhyTypeRequest{};
        setPhyTypeRequest.set_accesspointid(InterfaceName);
        setPhyTypeRequest.set_phytype(Dot11PhyType::Dot11PhyTypeB);

        WifiAccessPointSetPhyTypeResult setPhyTypeResult{};
        grpc::ClientContext setPhyTypeClientContext{};

        auto setPhyTypeStatus = client->WifiAccessPointSetPhyType(&setPhyTypeClientContext, setPhyTypeRequest, &setPhyTypeResult);
        REQUIRE(setPhyTypeStatus.ok());
        REQUIRE(setPhyTypeResult.accesspointid() == setPhyTypeRequest.accesspointid());
        REQUIRE(setPhyTypeResult.has_status());
        REQUIRE(setPhyTypeResult.status().code() == WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);
        REQUIRE(setPhyTypeResult.status().message().empty());
        REQUIRE(setPhyTypeResult.status().has_details() == false);
    }
}