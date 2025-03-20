#include <plog/Log.h>

#include "NetRemoteApiTrace.hxx"
#include "microsoft/net/remote/service/NetRemoteRfAttenuatorService.hxx"

using namespace Microsoft::Net::Remote::Service;
using namespace Microsoft::Net::Remote::Service::Tracing;
using namespace Microsoft::Net::Remote::RfAttenuator;

NetRemoteRfAttenuatorService::NetRemoteRfAttenuatorService(const NetRemoteRfAttenuatorConfiguration& configuration) :
    m_configuration(configuration)
{
    const NetRemoteApiTrace traceMe{};
    LOGI << std::format("NetRemoteRfAttenuatorService created with configuration: Type={}, Address={}, Port={}",
        static_cast<int>(configuration.Type),
        configuration.Address,
        configuration.Port);

    if (configuration.Type == RfAttenuatorType::Software) {
        m_attenuator = CreateSimulatedAttenuator();
        if (m_attenuator == nullptr) {
            LOGE << "Failed to create simulated RF attenuator";
            throw std::runtime_error("Failed to create simulated RF attenuator");
        }
    }
}

grpc::Status
NetRemoteRfAttenuatorService::IsEnabled([[maybe_unused]] ::grpc::ServerContext* context, [[maybe_unused]] const ::google::protobuf::Empty* request, ::google::protobuf::BoolValue* response)
{
    const NetRemoteApiTrace traceMe{};
    // Implementation of IsEnabled
    response->set_value(true);

    if (m_configuration.Type == RfAttenuatorType::None) {
        response->set_value(false);
    }

    LOGI << std::format("IsEnabled: {}", response->value());
    return grpc::Status::OK;
}

grpc::Status NetRemoteRfAttenuatorService::Reset([[maybe_unused]] ::grpc::ServerContext* context, [[maybe_unused]] const ::google::protobuf::Empty* request, ResetResult* response)
{
    const NetRemoteApiTrace traceMe{};
    std::scoped_lock attenuatorLock{ m_mutex };
    try {
        m_attenuator->Reset();
        response->mutable_status()->set_code(RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeSucceeded);
    } catch (const RfAttenuatorException& e) {
        LOGE << std::format("Failed to reset RF attenuator: {}", e.what());
        response->mutable_status()->set_code(RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeFailed);
        response->mutable_status()->set_message(e.what());
    }
    return grpc::Status::OK;
}

grpc::Status NetRemoteRfAttenuatorService::GetProperties([[maybe_unused]] ::grpc::ServerContext* context, [[maybe_unused]] const ::google::protobuf::Empty* request, GetPropertiesResult* response)
{
    const NetRemoteApiTrace traceMe{};
    std::scoped_lock attenuatorLock{ m_mutex };
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
    } catch (const RfAttenuatorException& e) {
        LOGE << std::format("Failed to get RF attenuator properties: {}", e.what());
        response->mutable_status()->set_code(RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeFailed);
    }
    return grpc::Status::OK;
}

grpc::Status NetRemoteRfAttenuatorService::GetAttenuationForChannel([[maybe_unused]] ::grpc::ServerContext* context, const GetAttenuationRequest* request, GetAttenuationResult* response)
{
    const NetRemoteApiTrace traceMe{};
    std::scoped_lock attenuatorLock{ m_mutex };
    try {
        auto attenuation = m_attenuator->GetAttenuationForChannel(request->channel());
        response->set_attenuationdbm(attenuation);

        response->mutable_status()->set_code(RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeSucceeded);
    } catch (const RfAttenuatorException& e) {
        LOGE << std::format("Failed to get RF attenuator channel {} attenuation: {}", request->channel(), e.what());
        response->mutable_status()->set_code(RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeFailed);
        response->mutable_status()->set_message(e.what());
    }
    return grpc::Status::OK;
}

grpc::Status NetRemoteRfAttenuatorService::SetAttenuationForChannel([[maybe_unused]] ::grpc::ServerContext* context, const SetAttenuationRequest* request, SetAttenuationResult* response)
{
    const NetRemoteApiTrace traceMe{};
    std::scoped_lock attenuatorLock{ m_mutex };
    try {
        auto result = m_attenuator->SetAttenuationForChannel(request->channel(), request->attenuationdbm());
        if (result) {
            response->mutable_status()->set_code(RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeSucceeded);
        } else {
            LOGE << std::format("False is returned when set RF attenuator channel {} attenuation", request->channel());
            response->mutable_status()->set_code(RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeFailed);
            response->mutable_status()->set_message("False is returned when set RF attenuator channel attenuation");
        }
    } catch (const RfAttenuatorException& e) {
        LOGE << std::format("Failed to set RF attenuator channel {} attenuation: {}", request->channel(), e.what());
        response->mutable_status()->set_code(RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeFailed);
        response->mutable_status()->set_message(e.what());
    }

    return grpc::Status::OK;
}

std::unique_ptr<IRfAttenuatorController>
NetRemoteRfAttenuatorService::CreateSimulatedAttenuator()
{
    // Implementation of CreateSimulatedAttenuator
    RfAttenuatorProperties properties{
        .Channels{ 1, 2, 3, 4 },
        .AttenuationRangeDbmMin = 0,
        .AttenuationRangeDbmMax = 100,
        .AttenuationStepDbmMin = 1,
        .AttenuationStepDbmMax = 5,
        .AttenuationAccuracyDbmMin = 1,
        .AttenuationAccuracyDbmMax = 1,
        .FrequencyBandwidthMHzMin = 0,
        .FrequencyBandwidthMHzMax = 6000,
        .SupportsSweep = false,
        .Identification = "Simulated Attenuator",
    };

    std::cout << "Creating software-based attenuator ... ";

    try {
        auto attenuator = RfAttenuatorFactory::TryCreateBasic("software", std::move(properties));
        std::cout << "succeeded" << std::endl;
        return attenuator;
    } catch (const RfAttenuatorException& e) {
        std::cout << "failed (" << e.what() << ")" << std::endl;
        return nullptr;
    }
}