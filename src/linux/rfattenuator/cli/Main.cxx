#include <exception>
#include <filesystem>
#include <format>
#include <functional>
#include <ios>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>

#include "RfAttenuator.hxx"
#include "RfAttenuatorFactory.hxx"
//#include "RfAttenuatorSoftwareSimulated.hxx"

namespace detail
{
void
DisplayAttenuatorProperties(const RfAttenuatorProperties& properties, std::ostream& out)
{
    // clang-format off
    out << properties.Identification.value_or("Unidentified Mode") << ", "
        << std::size(properties.Channels) << " channels, "
        << properties.FrequencyBandwidthMHzMin << "-" << properties.FrequencyBandwidthMHzMax << " MHz frequency bandwidth, "
        << properties.AttenuationRangeDbmMin << "-" << properties.AttenuationRangeDbmMax << " +/- "
        << properties.AttenuationStepDbmMin << "-" << properties.AttenuationStepDbmMax << " dBm attenuation, "
        << properties.AttenuationAccuracyDbmMin << "-" << properties.AttenuationAccuracyDbmMax << " dBm accuracy";
    if (properties.SupportsSweep)
    {
        out << " +Sweep";
    }
    out << std::endl;
    // clang-format on
}

std::tuple<bool, std::optional<RfAttenuatorProperties>>
ValidateAttenuatorInitialization(IRfAttenuatorController* attenuator)
{
    try {
        // Reset attenuator and clear state.
        std::cout << "Resetting attenuator ... ";
        attenuator->Reset();
        std::cout << "succeeded" << std::endl;

        // Obtain properties.
        std::cout << "Obtaining attenuator properties: ";
        auto properties = attenuator->GetProperties();
        detail::DisplayAttenuatorProperties(properties, std::cout);

        // Ensure at least 1 channel is supported.
        if (std::empty(properties.Channels)) {
            std::cout << "Attenuator does not support any channels, abandoning further sanity checks.";
            return { false, std::nullopt };
        }

        return { true, std::move(properties) };
    } catch (const RfAttenuatorException& e) {
        std::cout << "failed (" << e.what() << ")" << std::endl;
        return { false, std::nullopt };
    }
}

bool
ValidateAttenuatorSanityBasic(IRfAttenuatorController* attenuator)
{
    const auto [initializationSucceeded, properties] = ValidateAttenuatorInitialization(attenuator);
    if (!initializationSucceeded) {
        return false;
    }

    try {
        // Use first supported channel.
        uint32_t channel{ properties->Channels.front() };

        // Get attenuation for channel.
        std::cout << std::format("Getting attenuation for channel {}: ", channel);
        double attenuation = attenuator->GetAttenuationForChannel(channel);
        std::cout << std::format("{} dBm", attenuation) << std::endl;

        // Set attenuation for first channel.
        attenuation = (attenuation < properties->AttenuationRangeDbmMax) ? attenuation + 1 : attenuation - 1;
        std::cout << std::format("Setting attenuation for channel {} to {} dBm: ", channel, attenuation);
        auto setAttenuationSucceeded = attenuator->SetAttenuationForChannel(channel, attenuation);
        std::cout << ((setAttenuationSucceeded) ? "succeeded" : "failed") << std::endl;
        if (!setAttenuationSucceeded) {
            return false;
        }

        // Get attenuation and ensure the value was persisted.
        auto attenuationSet = attenuator->GetAttenuationForChannel(channel);
        std::cout << std::format("Re-reading attenuation for channel {}: {} dBm", channel, attenuationSet) << std::endl;

        return (attenuation == attenuationSet);
    } catch (const RfAttenuatorException& e) {
        std::cout << "failed (" << e.what() << ")" << std::endl;
        return false;
    }

    return true;
}

bool
ValidateAttenuatorSanityExtended(IRfAttenuatorController* attenuator)
{
    const auto [initializationSucceeded, properties] = ValidateAttenuatorInitialization(attenuator);
    if (!initializationSucceeded) {
        return false;
    }

    try {
        // Get attenuation for first channel.
        uint32_t channel{ properties->Channels.front() };
        std::cout << "Getting attenuation for channel " << channel << ": ";
        double attenuation = attenuator->GetAttenuationForChannel(channel);
        std::cout << attenuation << " dBm" << std::endl;

        // Set attenuation from min to max using min step.
        std::cout << "Increasing attenuation from min to max using minimum step value (" << properties->AttenuationRangeDbmMin << "-"
                  << properties->AttenuationRangeDbmMax << " dBm, +" << properties->AttenuationStepDbmMin << " dBm steps): ";
        attenuation = properties->AttenuationRangeDbmMin;
        while (attenuation < properties->AttenuationRangeDbmMax) {
            attenuation = std::clamp(
                attenuation + properties->AttenuationStepDbmMin, properties->AttenuationRangeDbmMin, properties->AttenuationRangeDbmMax);
            std::cout << attenuation << " ";

            if (!attenuator->SetAttenuationForChannel(channel, attenuation)) {
                std::cout << "*" << std::endl
                          << std::format("failed to set attenuation for channel {} to {}", channel, attenuation);
                return false;
            }
        }
        std::cout << std::endl;

        std::cout << "Decreasing attenuation from max to min using maximum step value (" << properties->AttenuationRangeDbmMax << "-"
                  << properties->AttenuationRangeDbmMin << " dBm, -" << properties->AttenuationStepDbmMax << " dBm steps): ";
        while (attenuation > properties->AttenuationRangeDbmMin) {
            attenuation = std::clamp(
                attenuation - properties->AttenuationStepDbmMax, properties->AttenuationRangeDbmMin, properties->AttenuationRangeDbmMax);
            std::cout << attenuation << " ";

            if (!attenuator->SetAttenuationForChannel(channel, attenuation)) {
                std::cout << "*" << std::endl
                          << std::format("failed to set attenuation for channel {} to {}", channel, attenuation);
                return false;
            }
        }
        std::cout << std::endl;
    } catch (const RfAttenuatorException& e) {
        std::cout << "failed (" << e.what() << ")" << std::endl;
        return false;
    }

    return true;
}

std::unique_ptr<IRfAttenuatorController>
CreateSimulatedAttenuator()
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
        auto attenuator = RfAttenuatorFactory::TryCreateBasic("software", std::move(properties));
        std::cout << "succeeded" << std::endl;
        return attenuator;
    } catch (const RfAttenuatorException& e) {
        std::cout << "failed (" << e.what() << ")" << std::endl;
        return nullptr;
    }
}

std::unique_ptr<IRfAttenuatorController>
CreateSocketAttenuator(std::string attenuatorName, std::string ipAddress, uint16_t port)
{
    RfAttenuatorConnectionArgumentsTcp args{
        .IpAddress = std::move(ipAddress),
        .Port = port,
    };

    std::cout << std::format(
                     "Creating socket-based attenuator {} @ {}:{}",
                     attenuatorName,
                     args.IpAddress,
                     args.Port)
              << " ... ";

    try {
        auto attenuator = RfAttenuatorFactory::TryCreateWithTcpConnection(attenuatorName, std::move(args));
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

void
ShowUsage(const char* programPath)
{
    const auto programName = std::filesystem::path(programPath).filename().string();

    // clang-format off
    std::cout << programName << " <attenuator type> [attenuator-type-args] [validation-mode]" << std::endl
               << std::endl
               << "    Attenuator Types: " << std::endl
               << "        'software'" << std::endl
               << "          'socket'" << std::endl
               << std::endl
               << "    Attenuator Type Arguments:" << std::endl
               << "         'software': none" << std::endl
               << "           'socket': <attenuator name> <ip address> <port>" << std::endl
               << std::endl
               << "    Validation Modes: " << std::endl
               << "           'basic' (default)" << std::endl
               << "        'extended'" << std::endl
               << std::endl;
    // clang-format on
}
} // namespace detail

int main(int argc, char* argv[])
{
    static const auto MinExpectedArguments = 1;
    static const auto MinExpectedArgumentsSoftware = 1;
    static const auto MinExpectedArgumentsSocket = 4;
    static const auto ValidationModeArgBasic = "basic";
    static const auto ValidationModeArgExtended = "extended";

    // Initialize winsock.
    // {
    //     WSADATA wsaData;
    //     const auto wsaStartupRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
    //     if (wsaStartupRet != 0) {
    //         std::wcerr << std::format("Failed to initialize winsock with code {}", wsaStartupRet) << std::endl;
    //         return -1;
    //     }
    // }

    // auto wsaCleanupOnClose = wil::scope_exit([] {
    //     WSACleanup();
    // });

    // RfAttenuatorSoftwareSimulatedFactory attenuatorFactory;
    // attenuatorFactory.Create();

    if (argc < (MinExpectedArguments + 1)) {
        std::cerr << std::format("error: {} arguments are expected, {} specified", MinExpectedArguments, argc - 1) << std::endl;
        detail::ShowUsage(argv[0]);
        return -1;
    }

    std::unique_ptr<IRfAttenuatorController> attenuator = nullptr;

    int argIndex{ 1 };
    int numArgsExpected{ 0 };
    std::string validationMode{ ValidationModeArgBasic };
    std::string attenuatorType{ argv[argIndex++] };

    if (attenuatorType == "software") {
        numArgsExpected = MinExpectedArgumentsSoftware;

        if (argc < numArgsExpected + 1) {
            std::cerr << std::format(
                              "error: {} arguments are expected for software-base attenuators, {} specified",
                              numArgsExpected,
                              argc - 1)
                       << std::endl;
            detail::ShowUsage(argv[0]);
            return -1;
        }

        if (argc > numArgsExpected + 1) {
            validationMode = argv[argIndex++];
        }

        attenuator = detail::CreateSimulatedAttenuator();
    } else if (attenuatorType == "socket") {
        numArgsExpected = MinExpectedArgumentsSocket;

        if (argc < numArgsExpected + 1) {
            std::cerr << std::format(
                              "error: {} arguments are expected for socket-based attenuators, {} specified",
                              numArgsExpected,
                              argc - 1)
                       << std::endl;
            detail::ShowUsage(argv[0]);
            return -1;
        }

        // Capture arguments.
        const std::string attenuatorName{ argv[argIndex++] };
        const std::string ipAddress{ argv[argIndex++] };
        const std::string portString{ argv[argIndex++] };
        if (argc > numArgsExpected + 1) {
            validationMode = argv[argIndex++];
        }

        // Parse port as 32-bit integer since 16-bit integers map to wchar_t,
        // which are interpreted as a single multi-byte character.
        std::istringstream ss{ portString };
        uint32_t port32{ 0 };
        ss >> port32;
        if (ss.fail()) {
            std::cerr << "failed to parse specified port32 '" << portString << "' as 16-bit integer; bailing" << std::endl;
            detail::ShowUsage(argv[0]);
            return -1;
        }

        uint16_t port = static_cast<uint16_t>(port32 & 0x0000FFFFu);
        attenuator = detail::CreateSocketAttenuator(attenuatorName, std::move(ipAddress), port);
    } else {
        std::cerr << "attenuator type not specified, exiting" << std::endl;
        detail::ShowUsage(argv[0]);
        return -1;
    }

    if (attenuator == nullptr) {
        std::cerr << std::format("failed to create specified {} attenuator, exiting", attenuatorType) << std::endl;
        return -1;
    }

    const std::function<bool(IRfAttenuatorController*)> validateSanity = (validationMode == ValidationModeArgExtended)
        ? detail::ValidateAttenuatorSanityExtended
        : detail::ValidateAttenuatorSanityBasic;

    const bool sanityCheckSucceeded = validateSanity(attenuator.get());
    std::cout << "Sanity check " << ((sanityCheckSucceeded) ? "succeeded" : "failed") << std::endl;

    return 0;
}
