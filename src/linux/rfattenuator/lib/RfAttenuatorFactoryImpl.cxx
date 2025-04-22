#include <format>
#include <iostream>

#include "RfAttenuatorAeroflexWeinschle83XX.hxx"
#include "RfAttenuatorExceptionImpl.hxx"
#include "RfAttenuatorSoftwareSimulated.hxx"
#include "RfAttenuatorTransportSocketLinux.hxx"
#include <strings/StringHelpers.hxx>
#include <microsoft/net/remote/service/RfAttenuatorFactory.hxx>

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

std::unique_ptr<IRfAttenuatorController>
RfAttenuatorFactory::CreateSimulatedSoftwareAttenuator()
{
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
        auto attenuator = TryCreateBasic("software", std::move(properties));
        std::cout << "succeeded" << std::endl;
        return attenuator;
    } catch (const RfAttenuatorException& e) {
        std::cout << "failed (" << e.what() << ")" << std::endl;
        return nullptr;
    }
}

std::unique_ptr<IRfAttenuatorController>
RfAttenuatorFactory::CreateSocketAfw83Attenuator(std::string ipAddress, uint16_t port)
{
    RfAttenuatorConnectionArgumentsTcp args{
        .IpAddress = std::move(ipAddress),
        .Port = port,
    };

    const auto attenuatorName = "AFW83";
    std::cout << std::format(
                     "Creating socket-based attenuator {} @ {}:{}",
                     attenuatorName,
                     args.IpAddress,
                     args.Port)
              << " ... ";

    try {
        auto attenuator = TryCreateWithTcpConnection(attenuatorName, std::move(args));
        std::cout << "succeeded" << std::endl;
        return attenuator;
    } catch (RfAttenuatorException& e) {
        std::cout << "failed (" << e.what() << ")" << std::endl;
        return nullptr;
    } catch (std::exception& e) {
        std::cout << "failed (" << e.what() << ")" << std::endl;
        return nullptr;
    }
}