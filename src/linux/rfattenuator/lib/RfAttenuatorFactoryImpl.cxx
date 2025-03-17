
#include <format>

#include "RfAttenuatorExceptionImpl.hxx"
#include "RfAttenuatorFactory.hxx"
#include "RfAttenuatorSoftwareSimulated.hxx"
#include "StringHelpers.hxx"

/* static */
std::unique_ptr<IRfAttenuatorController>
RfAttenuatorFactory::TryCreateBasic(std::string attenuatorName, RfAttenuatorProperties properties)
{
    // Try to find a supported factory based on the requested attenuator name.
    std::unique_ptr<IRfAttenuatorBasicFactory> factory = nullptr;
    if (Strings::IEquals(attenuatorName, "noop") || Strings::IEquals(attenuatorName, "test") ||
        Strings::IEquals(attenuatorName, "software") || Strings::IEquals(attenuatorName, "simulated")) {
        factory = std::make_unique<RfAttenuatorSoftwareSimulatedFactory>();
    }

    if (factory == nullptr) {
        throw RfAttenuatorExceptionImpl(std::format("failed to find supported {} basic attenuator", attenuatorName));
    }

    auto instance = factory->Create(std::move(properties));
    if (instance == nullptr) {
        throw RfAttenuatorExceptionImpl(std::format("failed to create {} basic attenuator", attenuatorName));
    }

    return instance;
}