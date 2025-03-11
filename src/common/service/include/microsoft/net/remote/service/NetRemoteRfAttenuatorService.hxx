#ifndef NET_REMOTE_RFATTENUATOR_SERVICE_HXX
#define NET_REMOTE_RFATTENUATOR_SERVICE_HXX
#include <grpcpp/server_context.h>
#include <grpcpp/support/status.h>
#include <microsoft/net/remote/protocol/NetRemoteRfAttenuator.grpc.pb.h>
#include <microsoft/net/remote/protocol/NetRemoteRfAttenuatorService.grpc.pb.h>
namespace Microsoft::Net::Remote::Service
{
/**
 * @brief Enum class representing the type of RF attenuator.
 */
enum class RfAttenuatorType {
    None = 0,
    Software = 1,
    Socket = 2,
};

struct NetRemoteRfAttenuatorConfiguration
{
    /**
     * @brief RF attenuator type.
     */
    RfAttenuatorType Type{ RfAttenuatorType::None };

    /**
     * @brief RF attenuator address.
     */
    std::string Address{};

    /**
     * @brief RF attenuator port.
     */
    uint32_t Port{ 0 };
};

/**
 * @brief Implementation of the NetRemoteRfAttenuator::Service gRPC service.
 */
class NetRemoteRfAttenuatorService :
    public NetRemoteRfAttenuator::Service
{
public:
    /**
     * @brief Construct a new NetRemoteRfAttenuatorService object.
     */
    NetRemoteRfAttenuatorService(const NetRemoteRfAttenuatorConfiguration& configuration);

private:
    grpc::Status
    IsEnabled(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::google::protobuf::BoolValue* response) override;

    grpc::Status
    Reset(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::Microsoft::Net::Remote::RfAttenuator::ResetResult* response) override;

    grpc::Status
    GetProperties(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::Microsoft::Net::Remote::RfAttenuator::GetPropertiesResult* response) override;

    grpc::Status
    GetAttenuationForChannel(::grpc::ServerContext* context, const ::Microsoft::Net::Remote::RfAttenuator::GetAttenuationRequest* request, ::Microsoft::Net::Remote::RfAttenuator::GetAttenuationResult* response) override;

    grpc::Status
    SetAttenuationForChannel(::grpc::ServerContext* context, const ::Microsoft::Net::Remote::RfAttenuator::SetAttenuationRequest* request, ::Microsoft::Net::Remote::RfAttenuator::SetAttenuationResult* response) override;

private:
    /**
     * @brief RF attenuator configuration.
     */
    NetRemoteRfAttenuatorConfiguration m_configuration;
};
} // namespace Microsoft::Net::Remote::Service
#endif // NET_REMOTE_RFATTENUATOR_SERVICE_HXX