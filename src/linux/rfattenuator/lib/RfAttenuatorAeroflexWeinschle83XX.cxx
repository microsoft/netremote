
#include <format>
#include <ranges>
#include <sstream>

#include "LogHelpers.hxx"
#include "RfAttenuatorAeroflexWeinschle83XX.hxx"
#include "RfAttenuatorImpl.hxx"

using LogHelpers::LogLevel;

namespace detail
{
/**
 * @brief Fixed properties for this unit.
 */
const RfAttenuatorProperties properties{
    .Channels{ 1, 2, 3, 4, 5, 6, 7, 8, 9 },
    .AttenuationRangeDbmMin = 0,
    .AttenuationRangeDbmMax = 103,
    .AttenuationStepDbmMin = 1,
    .AttenuationStepDbmMax = 1,
    .AttenuationAccuracyDbmMin = 1,
    .AttenuationAccuracyDbmMax = 1,
    .FrequencyBandwidthMHzMin = 0,
    .FrequencyBandwidthMHzMax = 6000,
    .SupportsSweep = false,
    .Identification = "Aeroflex-Weinschle 83XX Family Attenuator"
};

class CommandBuilder
{
public:
    CommandBuilder() = default;

    CommandBuilder&
    SetEcho(bool enable)
    {
        const char echoSetting = enable ? '1' : '0';
        AddCommand();
        m_queryBuffer << CommandTargetEcho << ' ' << echoSetting;
        return *this;
    }

    CommandBuilder&
    EchoDisable()
    {
        return SetEcho(false);
    }

    CommandBuilder&
    EchoEnable()
    {
        return SetEcho(true);
    }

    CommandBuilder&
    Reset()
    {
        AddCommand();
        m_queryBuffer << CommandTargetReset << CommandActionNone;
        return *this;
    }

    CommandBuilder&
    ReadAttenuation()
    {
        AddCommand();
        m_queryBuffer << CommandTargetAttenuation << CommandActionQuery;
        return *this;
    }

    CommandBuilder&
    GetIdentification()
    {
        AddCommand();
        m_queryBuffer << CommandTargetIdentify << CommandActionQuery;
        return *this;
    }

    CommandBuilder&
    WaitForOperationComplete()
    {
        AddCommand();
        m_queryBuffer << CommandTargetOperationComplete << CommandActionQuery;
        return *this;
    }

    CommandBuilder&
    SelectChannel(uint32_t channel)
    {
        AddCommand();
        m_queryBuffer << CommandTargetChannel << CommandActionSet << channel;
        return *this;
    }

    CommandBuilder&
    SetAttenuation(double attenuation)
    {
        AddCommand();
        m_queryBuffer << CommandTargetAttenuation << CommandActionSet << attenuation;
        return *this;
    }

    std::string
    Build()
    {
        m_queryBuffer << ProgramTerminator;
        auto query = m_queryBuffer.str();
        m_queryBuffer.clear();
        return query;
    }

    static std::vector<std::string>
    ParseResponse(const std::string& responseCombined)
    {
        // clang-format off

        // Documentation suggests multi-command responses are separated by a
        // comma, but in actuality, they are separated by the program terminator
        // ('\n'). Split the response using this as a delimeter.
        auto responseView = responseCombined
            | std::ranges::views::split(detail::CommandBuilder::ProgramTerminator)
            | std::ranges::views::transform([](auto&& response) {
                return std::string(&(*std::begin(response)), static_cast<std::size_t>(std::ranges::distance(response)));
            });

        // clang-format on

        std::vector<std::string> responses{};
        for (auto response : responseView) {
            // Trim newline characters from the responses.
            while (response.ends_with('\r') || response.ends_with('\n')) {
                response.resize(std::size(response) - 1);
            }

            if (!std::empty(response)) {
                responses.push_back(std::move(response));
            }
        }

        return responses;
    }

    // Command operators.
    static constexpr auto CommandMultiSeparator = ';';
    static constexpr auto CommandActionQuery = '?';
    static constexpr auto CommandActionSet = ' ';
    static constexpr auto CommandActionNone = ' ';
    static constexpr auto CommandCompletedIndicator = '1';

    // Command targets.
    static constexpr auto CommandTargetAttenuation = "ATTN";
    static constexpr auto CommandTargetChannel = "CHAN";
    static constexpr auto CommandTargetIdentify = "*IDN";
    static constexpr auto CommandTargetReset = "*RST";
    static constexpr auto CommandTargetOperationComplete = "*OPC";
    static constexpr auto CommandTargetWaitToContinue = "*WAI";
    static constexpr auto CommandTargetEcho = "ECHO";

    // Response bits.
    static constexpr auto ResponseMultiSeparator = '\r';

    // Common bits.
    static constexpr char ProgramTerminator = 0x0a;

private:
    void
    AddCommand()
    {
        if (m_commandCount++ > 0) {
            m_queryBuffer << CommandMultiSeparator << ' ';
        }
    }

private:
    uint32_t m_commandCount{ 0 };
    std::ostringstream m_queryBuffer;
};

} // namespace detail

/* static */
std::string
RfAttenuatorAeroflexWeinschle83XXProtocol::MakeResetRequest()
{
    // clang-format off
    auto requestBuffer = detail::CommandBuilder()
        .EchoEnable()
        .Reset()
        .Build();
    // clang-format on
    return requestBuffer;
}

/* static */
std::string
RfAttenuatorAeroflexWeinschle83XXProtocol::MakeGetAttenuatorPropertiesRequest()
{
    // clang-format off
    auto requestBuffer = detail::CommandBuilder()
        .GetIdentification()
        .Build();
    // clang-format on
    return requestBuffer;
}

/* static */
std::string
RfAttenuatorAeroflexWeinschle83XXProtocol::MakeGetAttenuationRequest(uint32_t channel)
{
    // clang-format off
    auto requestBuffer = detail::CommandBuilder()
        .SelectChannel(channel)
        .ReadAttenuation()
        .Build();
    // clang-format on
    return requestBuffer;
}

/* static */
std::string
RfAttenuatorAeroflexWeinschle83XXProtocol::MakeSetAttenuationRequest(uint32_t channel, double attenuation)
{
    // clang-format off
    auto requestBuffer = detail::CommandBuilder()
        .SelectChannel(channel)
        .SetAttenuation(attenuation)
        .WaitForOperationComplete()
        .Build();
    // clang-format on
    return requestBuffer;
}

/* static */
std::unique_ptr<RfAttenuatorResponseReset>
RfAttenuatorAeroflexWeinschle83XXProtocol::ParseResetResponse(
    [[maybe_unused]] const std::vector<std::string>& responses)
{
    auto responseReset = std::make_unique<RfAttenuatorResponseReset>();
    responseReset->Status = RfAttenuatorRequestStatus::Succeeded;
    // No parsing needed as the reset has no data associated with it.

    return responseReset;
}

/* static */
std::unique_ptr<RfAttenuatorResponseGetProperties>
RfAttenuatorAeroflexWeinschle83XXProtocol::ParseGetAttenuatorPropertiesResponse(
    const std::vector<std::string>& responses)
{
    auto responseProperties = std::make_unique<RfAttenuatorResponseGetProperties>();

    const auto& response = responses.back();
    auto identification = response;
    responseProperties->AttenuatorProperties = detail::properties;
    responseProperties->AttenuatorProperties.Identification = std::move(identification);
    responseProperties->Status = RfAttenuatorRequestStatus::Succeeded;

    return responseProperties;
}

/* static */
std::unique_ptr<RfAttenuatorResponseGetAttenuation>
RfAttenuatorAeroflexWeinschle83XXProtocol::ParseGetAttenuationResponse(
    const std::vector<std::string>& responses)
{
    auto responseGetAttenuation = std::make_unique<RfAttenuatorResponseGetAttenuation>(RfAttenuationChannelStatus{});

    const auto& response = responses.back();

    try {
        auto attenuation = std::stod(response);
        responseGetAttenuation->AttenuationStatus.Attenuation = attenuation;
        responseGetAttenuation->Status = RfAttenuatorRequestStatus::Succeeded;
    } catch (std::exception& e) {
        throw RfAttenuatorRequestException(std::format("failed to parse attenuation value from response ({})", e.what()));
    }

    return responseGetAttenuation;
}

/* static */
std::unique_ptr<RfAttenuatorResponseSetAttenuation>
RfAttenuatorAeroflexWeinschle83XXProtocol::ParseSetAttenuationResponse(
    const std::vector<std::string>& responses)
{
    auto responseSetAttenuation = std::make_unique<RfAttenuatorResponseSetAttenuation>();

    const auto& response = responses.back();
    if (response.ends_with(detail::CommandBuilder::CommandCompletedIndicator)) {
        responseSetAttenuation->Status = RfAttenuatorRequestStatus::Succeeded;
    } else {
        responseSetAttenuation->Status = RfAttenuatorRequestStatus::Failed;
    }

    return responseSetAttenuation;
}

std::unique_ptr<IRfAttenuatorController>
RfAttenuatorAeroflexWeinschle83XXFactory::Create(
    std::unique_ptr<RfAttenuatorTransportSocketLinux> transportTcp)
{
    auto traceMe{ LogHelpers::TraceFunction() };

    auto protocolAdapter = std::make_unique<RfAttenuatorAeroflexWeinschle83XXSocketProtocolAdapter>();
    auto rfAttenuator = std::make_unique<RfAttenuator<decltype(transportTcp)::element_type::MessageDataType>>(
        std::move(transportTcp), std::move(protocolAdapter));
    return rfAttenuator;
}

TcpTransportConfiguration
RfAttenuatorAeroflexWeinschle83XXFactory::GetTransportConfiguration()
{
    return TransportConfiguration;
}

std::vector<uint8_t>
RfAttenuatorAeroflexWeinschle83XXSocketProtocolAdapter::AdaptRequest(IRfAttenuatorRequest* request)
{
    using protocol = RfAttenuatorAeroflexWeinschle83XXProtocol;

    auto traceMe{ LogHelpers::TraceFunction(LogLevel::Trace) };

    std::string requestProtocol{};

    switch (request->Type) {
    case RfAttenuatorRequestType::Reset: {
        requestProtocol = protocol::MakeResetRequest();
        break;
    }
    case RfAttenuatorRequestType::GetAttenuatorProperties: {
        requestProtocol = protocol::MakeGetAttenuatorPropertiesRequest();
        break;
    }
    case RfAttenuatorRequestType::GetAttenuationValue: {
        const auto& requestGetAttenuation = *reinterpret_cast<const RfAttenuatorRequestGetAttenuation*>(request);
        requestProtocol = protocol::MakeGetAttenuationRequest(requestGetAttenuation.Channel);
        break;
    }
    case RfAttenuatorRequestType::SetAttenuationValue: {
        const auto& requestSetAttenuation = *reinterpret_cast<const RfAttenuatorRequestSetAttenuation*>(request);
        requestProtocol = protocol::MakeSetAttenuationRequest(requestSetAttenuation.Channel, requestSetAttenuation.Attenuation);
        break;
    }
    default: {
        throw RfAttenuatorRequestException(std::format("request type {} not supported", static_cast<uint32_t>(request->Type)));
    }
    }

    std::vector<uint8_t> requestAdapted{
        std::make_move_iterator(std::begin(requestProtocol)), std::make_move_iterator(std::end(requestProtocol))
    };

    return requestAdapted;
}

std::unique_ptr<IRfAttenuatorResponse>
RfAttenuatorAeroflexWeinschle83XXSocketProtocolAdapter::AdaptResponse(
    const IRfAttenuatorRequest* request, std::vector<uint8_t>& responseBuffer)
{
    using protocol = RfAttenuatorAeroflexWeinschle83XXProtocol;

    auto traceMe{ LogHelpers::TraceFunction(LogLevel::Trace) };

    // Response data is ASCII-encoded, so move it directly into a string.
    std::unique_ptr<IRfAttenuatorResponse> responseAdapted = nullptr;
    std::string response{ std::make_move_iterator(std::begin(responseBuffer)), std::make_move_iterator(std::end(responseBuffer)) };

    // Parse the combined response into a list of responses.
    auto responses = detail::CommandBuilder::ParseResponse(response);

    // Use the request as a hint to determine the type of response to adapt.
    switch (request->Type) {
    case RfAttenuatorRequestType::Reset: {
        auto responseReset = protocol::ParseResetResponse(responses);
        responseAdapted = std::move(responseReset);
        break;
    }
    case RfAttenuatorRequestType::GetAttenuatorProperties: {
        auto responseGetAttenuatorProperties = protocol::ParseGetAttenuatorPropertiesResponse(responses);
        responseAdapted = std::move(responseGetAttenuatorProperties);
        break;
    }
    case RfAttenuatorRequestType::GetAttenuationValue: {
        const auto& requestGetAttenuation = *reinterpret_cast<const RfAttenuatorRequestGetAttenuation*>(request);
        auto responseGetAttenuationValue = protocol::ParseGetAttenuationResponse(responses);
        if (responseGetAttenuationValue != nullptr) {
            responseGetAttenuationValue->AttenuationStatus.Channel = requestGetAttenuation.Channel;
        }
        responseAdapted = std::move(responseGetAttenuationValue);
        break;
    }
    case RfAttenuatorRequestType::SetAttenuationValue: {
        auto responseSetAttenuationValue = protocol::ParseSetAttenuationResponse(responses);
        responseAdapted = std::move(responseSetAttenuationValue);
        break;
    }
    default: {
        throw RfAttenuatorRequestException(
            std::format("response for request type {} not supported", static_cast<uint32_t>(request->Type)));
    }
    }

    return responseAdapted;
}
