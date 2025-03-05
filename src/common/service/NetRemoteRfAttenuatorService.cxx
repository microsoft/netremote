#include <plog/Log.h>

#include "NetRemoteApiTrace.hxx"
#include "microsoft/net/remote/service/NetRemoteRfAttenuatorService.hxx"

using namespace Microsoft::Net::Remote::Service;
using namespace Microsoft::Net::Remote::Service::Tracing;
using namespace Microsoft::Net::Remote::RfAttenuator;

grpc::Status
NetRemoteRfAttenuatorService::IsEnabled([[maybe_unused]] ::grpc::ServerContext* context, [[maybe_unused]] const ::google::protobuf::Empty* request, ::google::protobuf::BoolValue* response)
{
    const NetRemoteApiTrace traceMe{};
    // Implementation of IsEnabled
    response->set_value(true);
    return grpc::Status::OK;
}

grpc::Status NetRemoteRfAttenuatorService::Reset([[maybe_unused]] ::grpc::ServerContext* context, [[maybe_unused]] const ::google::protobuf::Empty* request, ResetResult* response)
{
    const NetRemoteApiTrace traceMe{};
    // Implementation of Reset
    response->mutable_status()->set_code(RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeSucceeded);
    return grpc::Status::OK;
}

grpc::Status NetRemoteRfAttenuatorService::GetProperties([[maybe_unused]] ::grpc::ServerContext* context, [[maybe_unused]] const ::google::protobuf::Empty* request, GetPropertiesResult* response)
{
    const NetRemoteApiTrace traceMe{};
    // Implementation of GetProperties
    response->mutable_status()->set_code(RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeSucceeded);
    return grpc::Status::OK;
}

grpc::Status NetRemoteRfAttenuatorService::GetAttenuationForChannel([[maybe_unused]] ::grpc::ServerContext* context, const GetAttenuationRequest* request, GetAttenuationResult* response)
{
    const NetRemoteApiTrace traceMe{};
    // Implementation of GetAttenuationForChannel
    response->mutable_status()->set_code(RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeSucceeded);
    return grpc::Status::OK;
}

grpc::Status NetRemoteRfAttenuatorService::SetAttenuationForChannel([[maybe_unused]] ::grpc::ServerContext* context, const SetAttenuationRequest* request, SetAttenuationResult* response)
{
    const NetRemoteApiTrace traceMe{};
    // Implementation of SetAttenuationForChannel
    response->mutable_status()->set_code(RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeSucceeded);
    return grpc::Status::OK;
}