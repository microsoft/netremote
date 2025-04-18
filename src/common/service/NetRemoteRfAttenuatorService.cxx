#include <plog/Log.h>

#include "NetRemoteApiTrace.hxx"
#include "microsoft/net/remote/service/NetRemoteRfAttenuatorService.hxx"

using namespace Microsoft::Net::Remote::Service;
using namespace Microsoft::Net::Remote::Service::Tracing;
using namespace Microsoft::Net::Remote::RfAttenuator;

NetRemoteRfAttenuatorService::NetRemoteRfAttenuatorService(const NetRemoteRfAttenuatorConfiguration& configuration, std::shared_ptr<IRfAttenuatorController> attenuator) :
    m_configuration(configuration)
{
    const NetRemoteApiTrace traceMe{};

    LOGI << std::format("NetRemoteRfAttenuatorService created with configuration: Type={}, Address={}, Port={}",
        static_cast<int>(configuration.Type),
        configuration.Address,
        configuration.Port);

    m_attenuator = attenuator;
    // if (configuration.Type == RfAttenuatorType::Software) {
    //     m_attenuator = CreateSimulatedAttenuator();
    //     if (m_attenuator == nullptr) {
    //         LOGE << "Failed to create simulated RF attenuator";
    //         throw std::runtime_error("Failed to create simulated RF attenuator");
    //     }
    // }
}

grpc::Status
NetRemoteRfAttenuatorService::IsEnabled([[maybe_unused]] ::grpc::ServerContext* context, [[maybe_unused]] const ::google::protobuf::Empty* request, ::google::protobuf::BoolValue* response)
{
    const NetRemoteApiTrace traceMe{};

    response->set_value(true);

    if (m_configuration.Type == RfAttenuatorType::None) {
        response->set_value(false);
    }

    LOGI << std::format("IsEnabled: {}", response->value());
    return grpc::Status::OK;
}

grpc::Status NetRemoteRfAttenuatorService::Reset([[maybe_unused]] ::grpc::ServerContext* context, [[maybe_unused]] const ::google::protobuf::Empty* request, ResetResult* response)
{
    std::scoped_lock attenuatorLock{ m_mutex };
    const NetRemoteApiTrace traceMe{};

    try {
        m_attenuator->Reset();
        response->mutable_status()->set_code(RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeSucceeded);
    } catch (const std::exception& e) {
        LOGE << std::format("Failed to reset RF attenuator: {}", e.what());
        response->mutable_status()->set_code(RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeFailed);
        response->mutable_status()->set_message(e.what());
    }
    return grpc::Status::OK;
}

grpc::Status NetRemoteRfAttenuatorService::GetProperties([[maybe_unused]] ::grpc::ServerContext* context, [[maybe_unused]] const ::google::protobuf::Empty* request, GetPropertiesResult* response)
{
    std::scoped_lock attenuatorLock{ m_mutex };
    const NetRemoteApiTrace traceMe{};

    try {
        auto properties = m_attenuator->GetProperties();
        *response->mutable_channels() = {
            std::make_move_iterator(std::begin(properties.Channels)),
            std::make_move_iterator(std::end(properties.Channels))
        };
        response->set_attenuationrangedbmmin(properties.AttenuationRangeDbmMin);
        response->set_attenuationrangedbmmax(properties.AttenuationRangeDbmMax);
        response->set_attenuationstepdbmmin(properties.AttenuationStepDbmMin);
        response->set_attenuationstepdbmmax(properties.AttenuationStepDbmMax);
        response->set_attenuationaccuracydbmmin(properties.AttenuationAccuracyDbmMin);
        response->set_attenuationaccuracydbmmax(properties.AttenuationAccuracyDbmMax);
        response->set_frequencybandwidthmhzmin(properties.FrequencyBandwidthMHzMin);
        response->set_frequencybandwidthmhzmax(properties.FrequencyBandwidthMHzMax);
        response->set_supportssweep(properties.SupportsSweep);
        if (properties.Identification.has_value()) {
            response->set_identification(properties.Identification.value());
        }

        response->mutable_status()->set_code(RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeSucceeded);
    } catch (const std::exception& e) {
        LOGE << std::format("Failed to get RF attenuator properties: {}", e.what());
        response->mutable_status()->set_code(RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeFailed);
    }
    return grpc::Status::OK;
}

grpc::Status NetRemoteRfAttenuatorService::GetAttenuationForChannel([[maybe_unused]] ::grpc::ServerContext* context, const GetAttenuationRequest* request, GetAttenuationResult* response)
{
    std::scoped_lock attenuatorLock{ m_mutex };
    const NetRemoteApiTrace traceMe{};

    try {
        auto attenuation = m_attenuator->GetAttenuationForChannel(request->channel());
        response->set_attenuationdbm(attenuation);

        response->mutable_status()->set_code(RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeSucceeded);
    } catch (const std::exception& e) {
        LOGE << std::format("Failed to get RF attenuator channel {} attenuation: {}", request->channel(), e.what());
        response->mutable_status()->set_code(RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeFailed);
        response->mutable_status()->set_message(e.what());
    }
    return grpc::Status::OK;
}

grpc::Status NetRemoteRfAttenuatorService::SetAttenuationForChannel([[maybe_unused]] ::grpc::ServerContext* context, const SetAttenuationRequest* request, SetAttenuationResult* response)
{
    std::scoped_lock attenuatorLock{ m_mutex };
    const NetRemoteApiTrace traceMe{};

    try {
        auto result = m_attenuator->SetAttenuationForChannel(request->channel(), request->attenuationdbm());

        if (result) {
            response->mutable_status()->set_code(RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeSucceeded);
        } else {
            LOGE << std::format("False is returned when set RF attenuator channel {} attenuation", request->channel());
            response->mutable_status()->set_code(RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeFailed);
            response->mutable_status()->set_message("False is returned when set RF attenuator channel attenuation");
        }
    } catch (const std::exception& e) {
        LOGE << std::format("Failed to set RF attenuator channel {} attenuation: {}", request->channel(), e.what());
        response->mutable_status()->set_code(RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeFailed);
        response->mutable_status()->set_message(e.what());
    }

    return grpc::Status::OK;
}