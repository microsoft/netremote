
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

namespace Microsoft::Net::Remote::Test
{
constexpr auto AllProtocols = magic_enum::enum_values<Microsoft::Net::Wifi::Ieee80211Protocol>();
constexpr auto AllBands = magic_enum::enum_values<Microsoft::Net::Wifi::Ieee80211FrequencyBand>();
} // namespace Microsoft::Net::Remote::Test

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

TEST_CASE("WifiAccessPointDisable API", "[basic][rpc][client][remote]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Remote::Test;
    using namespace Microsoft::Net::Remote::Wifi;
    using namespace Microsoft::Net::Wifi;
    using namespace Microsoft::Net::Wifi::Test;

    constexpr auto InterfaceName1{ "TestWifiAccessPointDisable1" };
    constexpr auto InterfaceName2{ "TestWifiAccessPointDisable2" };
    constexpr auto InterfaceNameInvalid{ "TestWifiAccessPointDisableInvalid" };

    auto apManagerTest = std::make_shared<AccessPointManagerTest>();
    Ieee80211AccessPointCapabilities apCapabilities{
        .Protocols{ std::cbegin(AllProtocols), std::cend(AllProtocols) }
    };

    auto apTest1 = std::make_shared<AccessPointTest>(InterfaceName1, apCapabilities);
    auto apTest2 = std::make_shared<AccessPointTest>(InterfaceName2, apCapabilities);
    apManagerTest->AddAccessPoint(apTest1);
    apManagerTest->AddAccessPoint(apTest2);

    NetRemoteServerConfiguration Configuration{
        .ServerAddress = RemoteServiceAddressHttp,
        .AccessPointManager = apManagerTest,
    };

    NetRemoteServer server{ Configuration };
    server.Run();

    auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
    auto client = NetRemote::NewStub(channel);

    SECTION("Can be called")
    {
        WifiAccessPointDisableRequest request{};
        request.set_accesspointid(InterfaceName1);

        WifiAccessPointDisableResult result{};
        grpc::ClientContext clientContext{};

        grpc::Status status;
        REQUIRE_NOTHROW(status = client->WifiAccessPointDisable(&clientContext, request, &result));
        REQUIRE(status.ok());
        REQUIRE(result.accesspointid() == request.accesspointid());
    }

    SECTION("Fails with invalid access point")
    {
        WifiAccessPointDisableRequest request{};
        request.set_accesspointid(InterfaceNameInvalid);

        WifiAccessPointDisableResult result{};
        grpc::ClientContext clientContext{};

        grpc::Status status;
        REQUIRE_NOTHROW(status = client->WifiAccessPointDisable(&clientContext, request, &result));
        REQUIRE(status.ok());
        REQUIRE(result.accesspointid() == request.accesspointid());
        REQUIRE(result.has_status());
        REQUIRE(result.status().code() == WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeAccessPointInvalid);
    }

    SECTION("Succeeds with valid access point")
    {
        WifiAccessPointDisableRequest request{};
        request.set_accesspointid(InterfaceName1);

        WifiAccessPointDisableResult result{};
        grpc::ClientContext clientContext{};

        grpc::Status status;
        REQUIRE_NOTHROW(status = client->WifiAccessPointDisable(&clientContext, request, &result));
        REQUIRE(status.ok());
        REQUIRE(result.accesspointid() == request.accesspointid());
        REQUIRE(result.has_status());
        REQUIRE(result.status().code() == WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);
        REQUIRE(apTest1->OperationalState == AccessPointOperationalState::Disabled);
    }

    SECTION("Succeeds with valid access point when already disabled")
    {
        for (auto i = 0; i < 2; ++i) {
            WifiAccessPointDisableRequest request{};
            request.set_accesspointid(InterfaceName1);

            WifiAccessPointDisableResult result{};
            grpc::ClientContext clientContext{};

            grpc::Status status;
            REQUIRE_NOTHROW(status = client->WifiAccessPointDisable(&clientContext, request, &result));
            REQUIRE(status.ok());
            REQUIRE(result.accesspointid() == request.accesspointid());
            REQUIRE(result.has_status());
            REQUIRE(result.status().code() == WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);
            REQUIRE(apTest1->OperationalState == AccessPointOperationalState::Disabled);
        }
    }
}

TEST_CASE("WifiAccessPointSetPhyType API", "[basic][rpc][client][remote]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Remote::Test;
    using namespace Microsoft::Net::Remote::Wifi;
    using namespace Microsoft::Net::Wifi;
    using namespace Microsoft::Net::Wifi::Test;

    constexpr auto InterfaceName{ "TestWifiAccessPointSetPhyType" };

    auto apManagerTest = std::make_shared<AccessPointManagerTest>();
    Ieee80211AccessPointCapabilities apCapabilities{
        .Protocols{ std::cbegin(AllProtocols), std::cend(AllProtocols) }
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

TEST_CASE("WifiAccessPointSetFrequencyBands API", "[basic][rpc][client][remote]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Remote::Test;
    using namespace Microsoft::Net::Remote::Wifi;
    using namespace Microsoft::Net::Wifi;
    using namespace Microsoft::Net::Wifi::Test;

    constexpr auto InterfaceNameAllBands{ "TestWifiAccessPointSetFrequencyBandsAll" };
    constexpr auto InterfaceNameBand2_4{ "TestWifiAccessPointSetFrequencyBands2_4" };
    constexpr auto InterfaceNameBand5_0{ "TestWifiAccessPointSetFrequencyBands5_0" };
    constexpr auto InterfaceNameBand6_0{ "TestWifiAccessPointSetFrequencyBands6_0" };

    auto apManagerTest = std::make_shared<AccessPointManagerTest>();

    Ieee80211AccessPointCapabilities apCapabilitiesBandsAll{
        .FrequencyBands{ std::cbegin(AllBands), std::cend(AllBands) }
    };
    Ieee80211AccessPointCapabilities apCapabilitiesBands2_4{
        .FrequencyBands{ Ieee80211FrequencyBand::TwoPointFourGHz }
    };
    Ieee80211AccessPointCapabilities apCapabilitiesBands5_0{
        .FrequencyBands{ Ieee80211FrequencyBand::FiveGHz }
    };
    Ieee80211AccessPointCapabilities apCapabilitiesBands6_0{
        .FrequencyBands{ Ieee80211FrequencyBand::SixGHz }
    };

    auto apTestBandsAll = std::make_shared<AccessPointTest>(InterfaceNameAllBands, std::move(apCapabilitiesBandsAll));
    auto apTestBands2_4 = std::make_shared<AccessPointTest>(InterfaceNameBand2_4, std::move(apCapabilitiesBands2_4));
    auto apTestBands5_0 = std::make_shared<AccessPointTest>(InterfaceNameBand5_0, std::move(apCapabilitiesBands5_0));
    auto apTestBands6_0 = std::make_shared<AccessPointTest>(InterfaceNameBand6_0, std::move(apCapabilitiesBands6_0));

    apManagerTest->AddAccessPoint(apTestBandsAll);
    apManagerTest->AddAccessPoint(apTestBands2_4);
    apManagerTest->AddAccessPoint(apTestBands5_0);
    apManagerTest->AddAccessPoint(apTestBands6_0);

    NetRemoteServerConfiguration Configuration{
        .ServerAddress = RemoteServiceAddressHttp,
        .AccessPointManager = apManagerTest,
    };

    NetRemoteServer server{ Configuration };
    server.Run();

    auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
    auto client = NetRemote::NewStub(channel);

    SECTION("Can be called")
    {
        WifiAccessPointSetFrequencyBandsRequest request{};
        request.set_accesspointid(InterfaceNameAllBands);
        request.mutable_frequencybands()->Add(Dot11FrequencyBand::Dot11FrequencyBand2_4GHz);
        request.mutable_frequencybands()->Add(Dot11FrequencyBand::Dot11FrequencyBand5_0GHz);

        grpc::ClientContext clientContext{};
        WifiAccessPointSetFrequencyBandsResult result{};
        auto status = client->WifiAccessPointSetFrequencyBands(&clientContext, request, &result);
        REQUIRE(status.ok());
        REQUIRE(result.accesspointid() == request.accesspointid());
        REQUIRE(result.has_status());
        REQUIRE(result.status().code() == WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);
    }

    SECTION("Empty frequency bands returns an error")
    {
        WifiAccessPointSetFrequencyBandsRequest request{};
        request.set_accesspointid(InterfaceNameAllBands);

        grpc::ClientContext clientContext{};
        WifiAccessPointSetFrequencyBandsResult result{};
        auto status = client->WifiAccessPointSetFrequencyBands(&clientContext, request, &result);
        REQUIRE(status.ok());
        REQUIRE(result.accesspointid() == request.accesspointid());
        REQUIRE(result.has_status());
        REQUIRE(result.status().code() == WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
    }

    SECTION("Invalid frequency band returns an error")
    {
        WifiAccessPointSetFrequencyBandsRequest request{};
        request.set_accesspointid(InterfaceNameAllBands);
        request.mutable_frequencybands()->Add(Dot11FrequencyBand::Dot11FrequencyBandUnknown);

        grpc::ClientContext clientContext{};
        WifiAccessPointSetFrequencyBandsResult result{};
        auto status = client->WifiAccessPointSetFrequencyBands(&clientContext, request, &result);
        REQUIRE(status.ok());
        REQUIRE(result.accesspointid() == request.accesspointid());
        REQUIRE(result.has_status());
        REQUIRE(result.status().code() == WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
    }

    SECTION("Invalid frequency band with other valid bands returns an error")
    {
        WifiAccessPointSetFrequencyBandsRequest request{};
        request.set_accesspointid(InterfaceNameAllBands);
        request.mutable_frequencybands()->Add(Dot11FrequencyBand::Dot11FrequencyBandUnknown);
        request.mutable_frequencybands()->Add(Dot11FrequencyBand::Dot11FrequencyBand5_0GHz);

        grpc::ClientContext clientContext{};
        WifiAccessPointSetFrequencyBandsResult result{};
        auto status = client->WifiAccessPointSetFrequencyBands(&clientContext, request, &result);
        REQUIRE(status.ok());
        REQUIRE(result.accesspointid() == request.accesspointid());
        REQUIRE(result.has_status());
        REQUIRE(result.status().code() == WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeInvalidParameter);
    }

    SECTION("Unsupported frequency band returns an error")
    {
        WifiAccessPointSetFrequencyBandsRequest request{};
        request.set_accesspointid(InterfaceNameBand2_4);
        request.mutable_frequencybands()->Add(Dot11FrequencyBand::Dot11FrequencyBand5_0GHz);

        grpc::ClientContext clientContext{};
        WifiAccessPointSetFrequencyBandsResult result{};
        auto status = client->WifiAccessPointSetFrequencyBands(&clientContext, request, &result);
        REQUIRE(status.ok());
        REQUIRE(result.accesspointid() == request.accesspointid());
        REQUIRE(result.has_status());
        REQUIRE(result.status().code() == WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeOperationNotSupported);
    }

    SECTION("Enabling multiple bands works")
    {
        WifiAccessPointSetFrequencyBandsRequest request{};
        request.set_accesspointid(InterfaceNameAllBands);
        request.mutable_frequencybands()->Add(Dot11FrequencyBand::Dot11FrequencyBand2_4GHz);
        request.mutable_frequencybands()->Add(Dot11FrequencyBand::Dot11FrequencyBand5_0GHz);
        request.mutable_frequencybands()->Add(Dot11FrequencyBand::Dot11FrequencyBand6_0GHz);

        grpc::ClientContext clientContext{};
        WifiAccessPointSetFrequencyBandsResult result{};
        auto status = client->WifiAccessPointSetFrequencyBands(&clientContext, request, &result);
        REQUIRE(status.ok());
        REQUIRE(result.accesspointid() == request.accesspointid());
        REQUIRE(result.has_status());
        REQUIRE(result.status().code() == WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);
    }
}
