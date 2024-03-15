
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include <catch2/catch_test_macros.hpp>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/impl/codegen/status.h>
#include <grpcpp/security/credentials.h>
#include <magic_enum.hpp>
#include <microsoft/net/remote/NetRemoteServer.hxx>
#include <microsoft/net/remote/NetRemoteServerConfiguration.hxx>
#include <microsoft/net/remote/protocol/NetRemoteService.grpc.pb.h>
#include <microsoft/net/remote/protocol/NetRemoteWifi.pb.h>
#include <microsoft/net/remote/protocol/WifiCore.pb.h>
#include <microsoft/net/wifi/AccessPointManager.hxx>
#include <microsoft/net/wifi/AccessPointOperationStatus.hxx>
#include <microsoft/net/wifi/Ieee80211.hxx>
#include <microsoft/net/wifi/Ieee80211AccessPointCapabilities.hxx>
#include <microsoft/net/wifi/test/AccessPointManagerTest.hxx>
#include <microsoft/net/wifi/test/AccessPointTest.hxx>

#include "TestNetRemoteCommon.hxx"

namespace Microsoft::Net::Remote::Test
{
constexpr auto AllPhyTypes = magic_enum::enum_values<Microsoft::Net::Wifi::Ieee80211PhyType>();
constexpr auto AllBands = magic_enum::enum_values<Microsoft::Net::Wifi::Ieee80211FrequencyBand>();
} // namespace Microsoft::Net::Remote::Test

using Microsoft::Net::Remote::Test::RemoteServiceAddressHttp;

TEST_CASE("WifiEnumerateAccessPoints API", "[basic][rpc][client][remote]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Remote::Wifi;
    using namespace Microsoft::Net::Wifi;

    using Microsoft::Net::Remote::Test::RemoteServiceAddressHttp;

    const NetRemoteServerConfiguration Configuration{
        .ServerAddress = RemoteServiceAddressHttp,
        .AccessPointManager = AccessPointManager::Create(),
    };

    NetRemoteServer server{ Configuration };
    server.Run();

    auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
    auto client = NetRemote::NewStub(channel);

    SECTION("Can be called")
    {
        const WifiEnumerateAccessPointsRequest request{};

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
        const WifiEnumerateAccessPointsRequest request{};

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
    using namespace Microsoft::Net::Remote::Test;
    using namespace Microsoft::Net::Remote::Wifi;
    using namespace Microsoft::Net::Wifi;
    using namespace Microsoft::Net::Wifi::Test;

    constexpr auto SsidName{ "TestWifiAccessPointEnable" };
    constexpr auto InterfaceName1{ "TestWifiAccessPointEnable1" };
    constexpr auto InterfaceName2{ "TestWifiAccessPointEnable2" };
    constexpr auto InterfaceNameInvalid{ "TestWifiAccessPointEnableInvalid" };

    auto apManagerTest = std::make_shared<AccessPointManagerTest>();
    const Ieee80211AccessPointCapabilities apCapabilities{
        .PhyTypes{ std::cbegin(AllPhyTypes), std::cend(AllPhyTypes) },
        .FrequencyBands{ std::cbegin(AllBands), std::cend(AllBands) }
    };

    auto apTest1 = std::make_shared<AccessPointTest>(InterfaceName1, apCapabilities);
    auto apTest2 = std::make_shared<AccessPointTest>(InterfaceName2, apCapabilities);
    apManagerTest->AddAccessPoint(apTest1);
    apManagerTest->AddAccessPoint(apTest2);

    const NetRemoteServerConfiguration Configuration{
        .ServerAddress = RemoteServiceAddressHttp,
        .AccessPointManager = apManagerTest,
    };

    NetRemoteServer server{ Configuration };
    server.Run();

    auto channel = grpc::CreateChannel(RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
    auto client = NetRemote::NewStub(channel);

    SECTION("Can be called")
    {
        Dot11CipherSuiteConfiguration dot11CipherSuiteConfigurationWpa1{};
        dot11CipherSuiteConfigurationWpa1.set_securityprotocol(Dot11SecurityProtocol::Dot11SecurityProtocolWpa);
        dot11CipherSuiteConfigurationWpa1.mutable_ciphersuites()->Add(Dot11CipherSuite::Dot11CipherSuiteCcmp256);

        Dot11AccessPointConfiguration apConfiguration{};
        apConfiguration.set_phytype(Dot11PhyType::Dot11PhyTypeA);
        apConfiguration.mutable_ssid()->set_name(SsidName);
        apConfiguration.mutable_pairwiseciphersuites()->Add(std::move(dot11CipherSuiteConfigurationWpa1));
        apConfiguration.mutable_authenticationalgorithms()->Add(Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmSharedKey);
        apConfiguration.mutable_frequencybands()->Add(Dot11FrequencyBand::Dot11FrequencyBand2_4GHz);
        apConfiguration.mutable_frequencybands()->Add(Dot11FrequencyBand::Dot11FrequencyBand5_0GHz);

        WifiAccessPointEnableRequest request{};
        request.set_accesspointid(InterfaceName1);
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

    SECTION("Fails with invalid access point")
    {
        WifiAccessPointEnableRequest request{};
        *request.mutable_configuration() = {};
        request.set_accesspointid(InterfaceNameInvalid);

        WifiAccessPointEnableResult result{};
        grpc::ClientContext clientContext{};

        grpc::Status status;
        REQUIRE_NOTHROW(status = client->WifiAccessPointEnable(&clientContext, request, &result));
        REQUIRE(status.ok());
        REQUIRE(result.accesspointid() == request.accesspointid());
        REQUIRE(result.has_status());
        REQUIRE(result.status().code() == WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeAccessPointInvalid);
    }

    SECTION("Succeeds without access point configuration if already configured")
    {
        Dot11CipherSuiteConfiguration dot11CipherSuiteConfigurationWpa1{};
        dot11CipherSuiteConfigurationWpa1.set_securityprotocol(Dot11SecurityProtocol::Dot11SecurityProtocolWpa);
        dot11CipherSuiteConfigurationWpa1.mutable_ciphersuites()->Add(Dot11CipherSuite::Dot11CipherSuiteCcmp256);

        // Perform initial enable with configuration.
        Dot11AccessPointConfiguration apConfiguration{};
        apConfiguration.set_phytype(Dot11PhyType::Dot11PhyTypeA);
        apConfiguration.mutable_ssid()->set_name(SsidName);
        apConfiguration.mutable_pairwiseciphersuites()->Add(std::move(dot11CipherSuiteConfigurationWpa1));
        apConfiguration.mutable_authenticationalgorithms()->Add(Dot11AuthenticationAlgorithm::Dot11AuthenticationAlgorithmSharedKey);
        apConfiguration.mutable_frequencybands()->Add(Dot11FrequencyBand::Dot11FrequencyBand2_4GHz);

        WifiAccessPointEnableRequest request{};
        request.set_accesspointid(InterfaceName1);
        *request.mutable_configuration() = std::move(apConfiguration);

        WifiAccessPointEnableResult result{};
        grpc::ClientContext clientContext{};

        auto status = client->WifiAccessPointEnable(&clientContext, request, &result);
        REQUIRE(status.ok());
        REQUIRE(result.accesspointid() == request.accesspointid());
        REQUIRE(result.has_status());
        REQUIRE(result.status().code() == WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);

        // Disable.
        {
            WifiAccessPointDisableRequest disableRequest{};
            disableRequest.set_accesspointid(InterfaceName1);
            WifiAccessPointDisableResult disableResult{};
            grpc::ClientContext disableClientContext{};

            grpc::Status disableStatus;
            REQUIRE_NOTHROW(disableStatus = client->WifiAccessPointDisable(&disableClientContext, disableRequest, &disableResult));

            REQUIRE(disableResult.has_status());
            REQUIRE(disableResult.status().code() == WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);
        }

        // Perform second enable without configuration.
        request.clear_configuration();
        result.Clear();
        grpc::ClientContext clientContextReenable{};
        status = client->WifiAccessPointEnable(&clientContextReenable, request, &result);
        REQUIRE(result.accesspointid() == request.accesspointid());
        REQUIRE(result.has_status());
        REQUIRE(result.status().code() == WifiAccessPointOperationStatusCode::WifiAccessPointOperationStatusCodeSucceeded);
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
    const Ieee80211AccessPointCapabilities apCapabilities{
        .PhyTypes{ std::cbegin(AllPhyTypes), std::cend(AllPhyTypes) }
    };

    auto apTest1 = std::make_shared<AccessPointTest>(InterfaceName1, apCapabilities);
    auto apTest2 = std::make_shared<AccessPointTest>(InterfaceName2, apCapabilities);
    apManagerTest->AddAccessPoint(apTest1);
    apManagerTest->AddAccessPoint(apTest2);

    const NetRemoteServerConfiguration Configuration{
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
    const Ieee80211AccessPointCapabilities apCapabilities{
        .PhyTypes{ std::cbegin(AllPhyTypes), std::cend(AllPhyTypes) }
    };
    auto apTest = std::make_shared<AccessPointTest>(InterfaceName, apCapabilities);
    apManagerTest->AddAccessPoint(apTest);

    const NetRemoteServerConfiguration Configuration{
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

    const Ieee80211AccessPointCapabilities apCapabilitiesBandsAll{
        .FrequencyBands{ std::cbegin(AllBands), std::cend(AllBands) }
    };
    const Ieee80211AccessPointCapabilities apCapabilitiesBands2_4{
        .FrequencyBands{ Ieee80211FrequencyBand::TwoPointFourGHz }
    };
    const Ieee80211AccessPointCapabilities apCapabilitiesBands5_0{
        .FrequencyBands{ Ieee80211FrequencyBand::FiveGHz }
    };
    const Ieee80211AccessPointCapabilities apCapabilitiesBands6_0{
        .FrequencyBands{ Ieee80211FrequencyBand::SixGHz }
    };

    auto apTestBandsAll = std::make_shared<AccessPointTest>(InterfaceNameAllBands, apCapabilitiesBandsAll);
    auto apTestBands2_4 = std::make_shared<AccessPointTest>(InterfaceNameBand2_4, apCapabilitiesBands2_4);
    auto apTestBands5_0 = std::make_shared<AccessPointTest>(InterfaceNameBand5_0, apCapabilitiesBands5_0);
    auto apTestBands6_0 = std::make_shared<AccessPointTest>(InterfaceNameBand6_0, apCapabilitiesBands6_0);

    apManagerTest->AddAccessPoint(apTestBandsAll);
    apManagerTest->AddAccessPoint(apTestBands2_4);
    apManagerTest->AddAccessPoint(apTestBands5_0);
    apManagerTest->AddAccessPoint(apTestBands6_0);

    const NetRemoteServerConfiguration Configuration{
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
