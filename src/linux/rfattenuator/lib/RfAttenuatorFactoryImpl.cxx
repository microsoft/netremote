
#include <format>

#include "RfAttenuatorAeroflexWeinschle83XX.hxx"
#include "RfAttenuatorExceptionImpl.hxx"
#include "RfAttenuatorFactory.hxx"
#include "RfAttenuatorSoftwareSimulated.hxx"
#include "RfAttenuatorTransportSocketLinux.hxx"
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

/* static */
std::unique_ptr<IRfAttenuatorController>
RfAttenuatorFactory::TryCreateWithTcpConnection(std::string attenuatorName, RfAttenuatorConnectionArgumentsTcp args)
{
    // Try to find a supported factory based on the requested attenuator name.
    std::unique_ptr<IRfAttenuatorWithTcpConnectionFactory> factory = nullptr;
    for (const auto& prefix : RfAttenuatorAeroflexWeinschle83XXFactory::SupportedNamePrefixes) {
        if (attenuatorName.starts_with(prefix)) {
            factory = std::make_unique<RfAttenuatorAeroflexWeinschle83XXFactory>();
            break;
        }
    }

    if (factory == nullptr) {
        throw RfAttenuatorExceptionImpl(std::format("failed to find supported {} attenuator with TCP transport", attenuatorName));
    }

    // Create TCP transport based on the specified arguments.
    auto configuration = factory->GetTransportConfiguration();
    auto transport{
        RfAttenuatorTransportSocketLinux::CreateWithTcpConnection(
            args.IpAddress, args.Port, configuration.ReceiveSize, configuration.ReceiveDelay, configuration.SettlingTime)
    };

    // Invoke attenuator-specific factory, passing the TCP transport.
    auto instance = factory->Create(std::move(transport));
    if (instance == nullptr) {
        throw RfAttenuatorExceptionImpl(std::format("failed to create {} attenuator with TCP transport", attenuatorName));
    }

    return instance;
}
