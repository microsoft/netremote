
#include <algorithm>
#include <format>
#include <iostream>
#include <vector>

#include <microsoft/net/remote/NetRemoteService.hxx>
#include <plog/Log.h>

using namespace Microsoft::Net::Remote::Service;

::grpc::Status
NetRemoteService::WifiConfigureAccessPoint([[maybe_unused]] ::grpc::ServerContext* context, const ::Microsoft::Net::Remote::Wifi::WifiConfigureAccessPointRequest* request, ::Microsoft::Net::Remote::Wifi::WifiConfigureAccessPointResult* response)
{
    using Microsoft::Net::Remote::Wifi::WifiAccessPointApplyConfigurationResultConfigurationItem;

    LOG_VERBOSE << std::format("Received WifiConfigureAccessPoint request for access point id {} with {} configurations\n", request->accesspointid(), request->configurations_size());

    response->set_accesspointid(request->accesspointid());

    std::vector<WifiAccessPointApplyConfigurationResultConfigurationItem> results(static_cast<std::size_t>(std::size(request->configurations())));
    std::ranges::transform(request->configurations(), std::begin(results), [&](const auto& configuration) {
        return WifiAccessPointApplyConfiguration(request->accesspointid(), configuration);
    });

    *response->mutable_configurationresults() = {
        std::make_move_iterator(std::begin(results)),
        std::make_move_iterator(std::end(results))
    };

    return grpc::Status::OK;
}

::grpc::Status
NetRemoteService::WifiEnumerateAccessPoints([[maybe_unused]] ::grpc::ServerContext* context, [[maybe_unused]] const ::Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsRequest* request, [[maybe_unused]] ::Microsoft::Net::Remote::Wifi::WifiEnumerateAccessPointsResult* response)
{
    LOG_VERBOSE << std::format("Received WifiEnumerateAccessPoints request\n");

    return grpc::Status::OK;
}

::Microsoft::Net::Remote::Wifi::WifiAccessPointApplyConfigurationResultConfigurationItem 
NetRemoteService::WifiAccessPointApplyConfiguration([[maybe_unused]] std::string_view accessPointId, [[maybe_unused]] const ::Microsoft::Net::Remote::Wifi::WifiAccessPointApplyConfigurationRequestConfigurationItem& configuredDesired)
{
    ::Microsoft::Net::Remote::Wifi::WifiAccessPointApplyConfigurationResultConfigurationItem result{};
    result.set_status(Microsoft::Net::Remote::Wifi::WifiAccessPointApplyConfigurationStatus::WifiAccessPointApplyConfigurationStatusSucceeded);
    // TODO: implement if design accepted
    return result;
}
