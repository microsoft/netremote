
#include <algorithm>
#include <format>
#include <iostream>
#include <iterator>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <google/protobuf/util/field_mask_util.h>
#include <grpcpp/create_channel.h>
#include <magic_enum.hpp>
#include <microsoft/net/remote/NetRemoteServer.hxx>
#include <microsoft/net/remote/protocol/NetRemoteService.grpc.pb.h>

#include "TestNetRemoteCommon.hxx"

TEST_CASE("WifiConfigureAccessPoint API", "[basic][rpc][client][remote]")
{
    using namespace google::protobuf::util;
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Remote::Wifi;

    using Microsoft::Net::Remote::Wifi::WifiAccessPointApplyConfigurationRequest;
    using Microsoft::Net::Remote::Wifi::WifiAccessPointApplyConfigurationRequestConfigurationItem;
    using Microsoft::Net::Remote::Wifi::WifiAccessPointApplyConfigurationResult;
    using Microsoft::Net::Wifi::AccessPointConfiguration;

    NetRemoteServer server{ Test::RemoteServiceAddressHttp };
    server.Run();

    auto channel = grpc::CreateChannel(Test::RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
    auto client = NetRemote::NewStub(channel);

    SECTION("Can be called")
    {
        for (const auto& phyType : magic_enum::enum_values<Microsoft::Net::Wifi::Dot11PhyType>()) {
            AccessPointConfiguration apConfiguration{};
            apConfiguration.set_phytype(phyType);

            WifiAccessPointApplyConfigurationRequest request{};
            request.set_accesspointid(std::format("TestWifiConfigureAccessPoint{}", magic_enum::enum_name(phyType)));

            // Build a list of configurations to apply, one for each radio band.
            std::vector<WifiAccessPointApplyConfigurationRequestConfigurationItem> configurationsToApply{ magic_enum::enum_count<Microsoft::Net::Wifi::RadioBand>() };
            std::ranges::transform(magic_enum::enum_values<Microsoft::Net::Wifi::RadioBand>(), std::begin(configurationsToApply), [&](const auto& band) {
                WifiAccessPointApplyConfigurationRequestConfigurationItem item{};
                item.set_band(band);
                item.mutable_configuration()->set_phytype(phyType);
                *item.mutable_configurationmask() = google::protobuf::util::FieldMaskUtil::GetFieldMaskForAllFields<AccessPointConfiguration>();

                return item;
            });

            // Assign the configurations to the request.
            *request.mutable_configurations() = {
                std::make_move_iterator(std::begin(configurationsToApply)),
                std::make_move_iterator(std::end(configurationsToApply))
            };

            WifiAccessPointApplyConfigurationResult result{};
            grpc::ClientContext clientContext{};

            auto status = client->WifiConfigureAccessPoint(&clientContext, request, &result);

            REQUIRE(status.ok());
            REQUIRE(result.accesspointid() == request.accesspointid());
            REQUIRE(std::size(result.configurationresults()) == std::size(request.configurations()));

            for (const auto& configurationResult : result.configurationresults()) {
                REQUIRE(configurationResult.status() == WifiAccessPointApplyConfigurationStatus::WifiAccessPointApplyConfigurationStatusSucceeded);
            }
        }
    }
}

TEST_CASE("WifiEnumerateAccessPoints API", "[basic][rpc][client][remote]")
{
    using namespace Microsoft::Net::Remote;
    using namespace Microsoft::Net::Remote::Service;
    using namespace Microsoft::Net::Remote::Wifi;

    NetRemoteServer server{ Test::RemoteServiceAddressHttp };
    server.Run();

    auto channel = grpc::CreateChannel(Test::RemoteServiceAddressHttp, grpc::InsecureChannelCredentials());
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
