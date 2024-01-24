
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

    constexpr auto SsidName{ "TestWifiAccessPointSetPhyType" };

    NetRemoteServerConfiguration Configuration{
        .ServerAddress = RemoteServiceAddressHttp,
        .AccessPointManager = AccessPointManager::Create(),
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
    request.set_accesspointid("TestWifiAccessPointSetPhyType");
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
        setPhyTypeRequest.set_accesspointid("TestWifiAccessPointSetPhyType");
        setPhyTypeRequest.set_phytype(Dot11PhyType::Dot11PhyTypeB);

        WifiAccessPointSetPhyTypeResult setPhyTypeResult{};
        grpc::ClientContext setPhyTypeClientContext{};

        auto setPhyTypeStatus = client->WifiAccessPointSetPhyType(&setPhyTypeClientContext, setPhyTypeRequest, &setPhyTypeResult);
        REQUIRE(setPhyTypeStatus.ok());
        REQUIRE(setPhyTypeResult.accesspointid() == setPhyTypeRequest.accesspointid());
        REQUIRE(setPhyTypeResult.has_status());
    }
}

TEST_CASE("WifiAccessPointSetAuthenticationConfiguration API", "[basic][rpc][client][remote]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Remote::Wifi;
    using namespace Microsoft::Net::Wifi;

    constexpr auto SsidName{ "TestWifiAccessPointSetAuthenticationConfiguration" };

    NetRemoteServerConfiguration Configuration{
        .ServerAddress = RemoteServiceAddressHttp,
        .AccessPointManager = AccessPointManager::Create(),
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
    request.set_accesspointid("TestWifiAccessPointSetAuthenticationConfiguration");
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
        Dot11AccessPointAuthenticationConfiguration apAuthConfiguration{};

        // Add the first AKM suite.
        auto* akmSuiteConfiguration1 = apAuthConfiguration.mutable_akmsuites()->Add();
        akmSuiteConfiguration1->set_authenticationalgorithm(Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmSae);
        akmSuiteConfiguration1->set_akmsuite(Dot11AkmSuite::Dot11AkmSuiteSae);

        Dot11SharedKey sharedKey1{};
        sharedKey1.set_passphrase("passphrase1");
        Dot11AkmSuiteConfigurationSharedKey akmSuiteConfigurationSharedKey1{};
        *akmSuiteConfigurationSharedKey1.mutable_sharedkey() = std::move(sharedKey1);
        *akmSuiteConfiguration1->mutable_configurationsharedkey() = std::move(akmSuiteConfigurationSharedKey1);

        // Add the second AKM suite.
        auto* akmSuiteConfiguration2 = apAuthConfiguration.mutable_akmsuites()->Add();
        akmSuiteConfiguration2->set_authenticationalgorithm(Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmSae);
        akmSuiteConfiguration2->set_akmsuite(Dot11AkmSuite::Dot11AkmSuiteFtSae);

        Dot11SharedKey sharedKey2{};
        sharedKey2.set_passphrase("passphrase2");
        Dot11AkmSuiteConfigurationSharedKey akmSuiteConfigurationSharedKey2{};
        *akmSuiteConfigurationSharedKey2.mutable_sharedkey() = std::move(sharedKey2);
        *akmSuiteConfiguration2->mutable_configurationsharedkey() = std::move(akmSuiteConfigurationSharedKey2);

        WifiAccessPointSetAuthenticationConfigurationRequest setAuthConfigurationRequest{};
        setAuthConfigurationRequest.set_accesspointid("TestWifiAccessPointSetAuthenticationConfiguration");
        *setAuthConfigurationRequest.mutable_authenticationconfiguration() = std::move(apAuthConfiguration);

        WifiAccessPointSetAuthenticationConfigurationResult setAuthConfigurationResult{};
        grpc::ClientContext setAuthConfigurationClientContext{};

        auto setAuthConfigurationStatus = client->WifiAccessPointSetAuthenticationConfiguration(&setAuthConfigurationClientContext, setAuthConfigurationRequest, &setAuthConfigurationResult);
        REQUIRE(setAuthConfigurationStatus.ok());
        REQUIRE(setAuthConfigurationResult.accesspointid() == setAuthConfigurationRequest.accesspointid());
        REQUIRE(setAuthConfigurationResult.has_status());
    }
}

TEST_CASE("WifiAccessPointSetFrequencyBands API", "[basic][rpc][client][remote]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Remote::Wifi;
    using namespace Microsoft::Net::Wifi;

    constexpr auto SsidName{ "TestWifiAccessPointSetFrequencyBands" };

    NetRemoteServerConfiguration Configuration{
        .ServerAddress = RemoteServiceAddressHttp,
        .AccessPointManager = AccessPointManager::Create(),
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
    request.set_accesspointid("TestWifiAccessPointSetFrequencyBands");
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
        WifiAccessPointSetFrequencyBandsRequest setFrequencyBandsRequest{};
        setFrequencyBandsRequest.set_accesspointid("TestWifiAccessPointSetFrequencyBands");
        setFrequencyBandsRequest.mutable_frequencybands()->Add(Dot11FrequencyBand::Dot11FrequencyBand6_0GHz);

        WifiAccessPointSetFrequencyBandsResult setFrequencyBandsResult{};
        grpc::ClientContext setFrequencyBandsClientContext{};

        auto setFrequencyBandsStatus = client->WifiAccessPointSetFrequencyBands(&setFrequencyBandsClientContext, setFrequencyBandsRequest, &setFrequencyBandsResult);
        REQUIRE(setFrequencyBandsStatus.ok());
        REQUIRE(setFrequencyBandsResult.accesspointid() == setFrequencyBandsRequest.accesspointid());
        REQUIRE(setFrequencyBandsResult.has_status());
        REQUIRE(setFrequencyBandsResult.status().code() == WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);
        REQUIRE(setFrequencyBandsResult.status().message().empty());
        REQUIRE(setFrequencyBandsResult.status().has_details() == false);
    }
}
