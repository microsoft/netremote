
#include <format>

#include "RfAttenuatorExceptionImpl.hxx"
#include "RfAttenuatorSoftwareSimulated.hxx"
#include <microsoft/net/remote/service/RfAttenuatorFactory.hxx>
#include <strings/StringHelpers.hxx>

/* static */
std::unique_ptr<IRfAttenuatorController>
RfAttenuatorFactory::TryCreateBasic(std::string attenuatorName, RfAttenuatorProperties properties)
{
    // Try to find a supported factory based on the requested attenuator name.
    std::unique_ptr<IRfAttenuatorBasicFactory> factory = nullptr;
    if (Strings::CaseInsensitiveStringEquals(attenuatorName, "noop") || Strings::CaseInsensitiveStringEquals(attenuatorName, "test") ||
        Strings::CaseInsensitiveStringEquals(attenuatorName, "software") || Strings::CaseInsensitiveStringEquals(attenuatorName, "simulated")) {
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