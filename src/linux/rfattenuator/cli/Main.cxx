#include <exception>
#include <filesystem>
#include <format>
#include <functional>
#include <ios>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>

#include <google/protobuf/empty.pb.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <microsoft/net/remote/protocol/NetRemoteRfAttenuator.grpc.pb.h>
#include <microsoft/net/remote/protocol/NetRemoteRfAttenuatorService.grpc.pb.h>
#include <microsoft/net/remote/service/RfAttenuator.hxx>
#include <microsoft/net/remote/service/RfAttenuatorFactory.hxx>
// #include "RfAttenuatorSoftwareSimulated.hxx"

namespace detail
{
struct RfAttenuatorNetRemoteException : public RfAttenuatorException
{
    explicit RfAttenuatorNetRemoteException(std::string message) :
        Message(std::move(message))
    {
    }

    const char*
    what() const noexcept override
    {
        return std::data(Message);
    }

    std::string Message;
};

struct RfAttenuatorNetRemoteController : public IRfAttenuatorController
{
    explicit RfAttenuatorNetRemoteController(std::string address) :
        m_address(std::move(address)),
        m_channel(grpc::CreateChannel(m_address, grpc::InsecureChannelCredentials())),
        m_client(Microsoft::Net::Remote::Service::NetRemoteRfAttenuator::NewStub(m_channel))
    {
        if (m_channel == nullptr || m_client == nullptr) {
            throw RfAttenuatorNetRemoteException(std::format("failed to create netremote client for '{}'", m_address));
        }
    }

    void
    Reset() override
    {
        const google::protobuf::Empty request{ };
        Microsoft::Net::Remote::RfAttenuator::ResetResult response{ };
        grpc::ClientContext clientContext{ };

        auto status = m_client->Reset(&clientContext, request, &response);
        ThrowIfRpcFailed(status, "Reset");
        ThrowIfOperationFailed(response.status(), "Reset");
    }

    RfAttenuatorProperties
    GetProperties() override
    {
        const google::protobuf::Empty request{ };
        Microsoft::Net::Remote::RfAttenuator::GetPropertiesResult response{ };
        grpc::ClientContext clientContext{ };

        auto status = m_client->GetProperties(&clientContext, request, &response);
        ThrowIfRpcFailed(status, "GetProperties");
        ThrowIfOperationFailed(response.status(), "GetProperties");

        std::vector<uint32_t> channels{ };
        channels.reserve(static_cast<size_t>(response.channels_size()));
        for (const auto channel : response.channels()) {
            channels.emplace_back(channel);
        }

        return RfAttenuatorProperties{
            .Channels = std::move(channels),
            .AttenuationRangeDbmMin = response.attenuationrangedbmmin(),
            .AttenuationRangeDbmMax = response.attenuationrangedbmmax(),
            .AttenuationStepDbmMin = response.attenuationstepdbmmin(),
            .AttenuationStepDbmMax = response.attenuationstepdbmmax(),
            .AttenuationAccuracyDbmMin = response.attenuationaccuracydbmmin(),
            .AttenuationAccuracyDbmMax = response.attenuationaccuracydbmmax(),
            .FrequencyBandwidthMHzMin = response.frequencybandwidthmhzmin(),
            .FrequencyBandwidthMHzMax = response.frequencybandwidthmhzmax(),
            .SupportsSweep = response.supportssweep(),
            .Identification = response.identification(),
        };
    }

    double
    GetAttenuationForChannel(uint32_t channel) override
    {
        Microsoft::Net::Remote::RfAttenuator::GetAttenuationRequest request{ };
        request.set_channel(channel);

        Microsoft::Net::Remote::RfAttenuator::GetAttenuationResult response{ };
        grpc::ClientContext clientContext{ };

        auto status = m_client->GetAttenuationForChannel(&clientContext, request, &response);
        ThrowIfRpcFailed(status, "GetAttenuationForChannel");
        ThrowIfOperationFailed(response.status(), "GetAttenuationForChannel");

        return response.attenuationdbm();
    }

    bool
    SetAttenuationForChannel(uint32_t channel, double attenuation) override
    {
        Microsoft::Net::Remote::RfAttenuator::SetAttenuationRequest request{ };
        request.set_channel(channel);
        request.set_attenuationdbm(attenuation);

        Microsoft::Net::Remote::RfAttenuator::SetAttenuationResult response{ };
        grpc::ClientContext clientContext{ };

        auto status = m_client->SetAttenuationForChannel(&clientContext, request, &response);
        ThrowIfRpcFailed(status, "SetAttenuationForChannel");
        ThrowIfOperationFailed(response.status(), "SetAttenuationForChannel");

        return true;
    }

    static void
    ThrowIfRpcFailed(const grpc::Status& status, std::string_view operation)
    {
        if (!status.ok()) {
            throw RfAttenuatorNetRemoteException(
                std::format("{} RPC failed ({}: {})", operation, static_cast<int>(status.error_code()), status.error_message()));
        }
    }

    static void
    ThrowIfOperationFailed(const Microsoft::Net::Remote::RfAttenuator::RfAttenuatorOperationStatus& status, std::string_view operation)
    {
        if (status.code() != Microsoft::Net::Remote::RfAttenuator::RfAttenuatorOperationStatusCode::RfAttenuatorOperationStatusCodeSucceeded) {
            throw RfAttenuatorNetRemoteException(std::format(
                "{} operation failed (code={}, message='{}')",
                operation,
                static_cast<int>(status.code()),
                status.has_message() ? status.message() : ""));
        }
    }

private:
    std::string m_address{ };
    std::shared_ptr<grpc::Channel> m_channel{ };
    std::unique_ptr<Microsoft::Net::Remote::Service::NetRemoteRfAttenuator::Stub> m_client{ };
};

bool
TryParsePort(const std::string& portString, uint16_t& port)
{
    std::istringstream ss{ portString };
    uint32_t parsedPort{ 0 };
    ss >> parsedPort;

    if (!ss || !ss.eof() || parsedPort > std::numeric_limits<uint16_t>::max()) {
        return false;
    }

    port = static_cast<uint16_t>(parsedPort);
    return true;
}

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

void
ShowUsage(const char* programPath)
{
    const auto programName = std::filesystem::path(programPath).filename().string();

    // clang-format off
    std::cout << programName << " <attenuator type> [attenuator-type-args] [command]" << std::endl
               << std::endl
               << "    Attenuator Types: " << std::endl
               << "        'software'" << std::endl
               << "          'socket'" << std::endl
               << "       'netremote'" << std::endl
               << std::endl
               << "    Attenuator Type Arguments:" << std::endl
               << "         'software': none" << std::endl
               << "           'socket': AFW83 <ip address> <port>" << std::endl
               << "        'netremote': <ip address> <port>" << std::endl
               << std::endl
               << "    Commands (optional): " << std::endl
               << "              'get': get <channel> - Get attenuation for a channel" << std::endl
               << "              'set': set <channel> <attenuation> - Set attenuation for a channel" << std::endl
               << "            'basic': Run basic validation (default if no command specified)" << std::endl
               << "         'extended': Run extended validation" << std::endl
               << std::endl;
    // clang-format on
}
} // namespace detail

int
main(int argc, char* argv[])
{
    static const auto MinExpectedArguments = 1;
    static const auto MinExpectedArgumentsSoftware = 1;
    static const auto MinExpectedArgumentsSocket = 4;
    static const auto MinExpectedArgumentsNetRemote = 3;
    static const auto CommandArgBasic = "basic";
    static const auto CommandArgExtended = "extended";
    static const auto CommandArgGet = "get";
    static const auto CommandArgSet = "set";

    if (argc < (MinExpectedArguments + 1)) {
        std::cerr << std::format("error: {} arguments are expected, {} specified", MinExpectedArguments, argc - 1) << std::endl;
        detail::ShowUsage(argv[0]);
        return -1;
    }

    std::unique_ptr<IRfAttenuatorController> attenuator = nullptr;

    int argIndex{ 1 };
    int numArgsExpected{ 0 };
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

        attenuator = RfAttenuatorFactory::CreateSimulatedSoftwareAttenuator();
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

        uint16_t port{ 0 };
        if (!detail::TryParsePort(portString, port)) {
            std::cerr << std::format("invalid socket attenuator port '{}'", portString) << std::endl;
            detail::ShowUsage(argv[0]);
            return -1;
        }

        if (attenuatorName == "AFW83") {
            attenuator = RfAttenuatorFactory::CreateSocketAfw83Attenuator(std::move(ipAddress), port);
        }
        else {
            std::cerr << std::format("unknown socket-based attenuator name '{}'", attenuatorName) << std::endl;
            detail::ShowUsage(argv[0]);
            return -1;
        }
    } else if (attenuatorType == "netremote") {
        numArgsExpected = MinExpectedArgumentsNetRemote;

        if (argc < numArgsExpected + 1) {
            std::cerr << std::format(
                             "error: {} arguments are expected for netremote attenuators, {} specified",
                             numArgsExpected,
                             argc - 1)
                      << std::endl;
            detail::ShowUsage(argv[0]);
            return -1;
        }

        const std::string ipAddress{ argv[argIndex++] };
        const std::string portString{ argv[argIndex++] };

        uint16_t port{ 0 };
        if (!detail::TryParsePort(portString, port)) {
            std::cerr << std::format("invalid netremote server port '{}'", portString) << std::endl;
            detail::ShowUsage(argv[0]);
            return -1;
        }

        const auto address = std::format("{}:{}", ipAddress, port);
        std::cout << std::format("Creating netremote attenuator client @ {} ... ", address);

        try {
            attenuator = std::make_unique<detail::RfAttenuatorNetRemoteController>(address);
            std::cout << "succeeded" << std::endl;
        } catch (const std::exception& e) {
            std::cout << std::format("failed ({})", e.what()) << std::endl;
            return -1;
        }
    } else {
        std::cerr << "attenuator type not specified, exiting" << std::endl;
        detail::ShowUsage(argv[0]);
        return -1;
    }

    if (attenuator == nullptr) {
        std::cerr << std::format("failed to create specified {} attenuator, exiting", attenuatorType) << std::endl;
        return -1;
    }

    const std::string command{ (argc > argIndex) ? argv[argIndex++] : CommandArgBasic };

    if (command == CommandArgGet) {
        // get <channel>
        if (argc != argIndex + 1) {
            std::cerr << "error: 'get' command requires exactly one argument: <channel>" << std::endl;
            detail::ShowUsage(argv[0]);
            return -1;
        }

        uint32_t channel{ 0 };
        try {
            channel = static_cast<uint32_t>(std::stoul(argv[argIndex]));
        } catch (const std::exception& e) {
            std::cerr << std::format("error: invalid channel '{}': {}", argv[argIndex], e.what()) << std::endl;
            return -1;
        }

        try {
            double attenuation = attenuator->GetAttenuationForChannel(channel);
            std::cout << std::format("Attenuation for channel {}: {} dBm", channel, attenuation) << std::endl;
            return 0;
        } catch (const std::exception& e) {
            std::cerr << std::format("error: failed to get attenuation for channel {}: {}", channel, e.what()) << std::endl;
            return -1;
        }
    }

    if (command == CommandArgSet) {
        // set <channel> <attenuation>
        if (argc != argIndex + 2) {
            std::cerr << "error: 'set' command requires exactly two arguments: <channel> <attenuation>" << std::endl;
            detail::ShowUsage(argv[0]);
            return -1;
        }

        uint32_t channel{ 0 };
        double attenuation{ 0.0 };

        try {
            channel = static_cast<uint32_t>(std::stoul(argv[argIndex]));
            attenuation = std::stod(argv[argIndex + 1]);
        } catch (const std::exception& e) {
            std::cerr << std::format("error: invalid arguments: {}", e.what()) << std::endl;
            return -1;
        }

        try {
            std::cout << std::format("Setting attenuation for channel {} to {} dBm: ", channel, attenuation);
            bool succeeded = attenuator->SetAttenuationForChannel(channel, attenuation);
            std::cout << ((succeeded) ? "succeeded" : "failed") << std::endl;
            return succeeded ? 0 : -1;
        } catch (const std::exception& e) {
            std::cerr << std::format("error: failed to set attenuation: {}", e.what()) << std::endl;
            return -1;
        }
    }

    if (command != CommandArgBasic && command != CommandArgExtended) {
        std::cerr << std::format("error: unknown command '{}'", command) << std::endl;
        detail::ShowUsage(argv[0]);
        return -1;
    }

    if (argc != argIndex) {
        std::cerr << std::format("error: unexpected extra arguments provided for command '{}'", command) << std::endl;
        detail::ShowUsage(argv[0]);
        return -1;
    }

    const std::function<bool(IRfAttenuatorController*)> validateSanity = (command == CommandArgExtended)
        ? detail::ValidateAttenuatorSanityExtended
        : detail::ValidateAttenuatorSanityBasic;

    const bool sanityCheckSucceeded = validateSanity(attenuator.get());
    std::cout << "Sanity check " << ((sanityCheckSucceeded) ? "succeeded" : "failed") << std::endl;

    return 0;
}