#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "RfAttenuatorSubFactory.hxx"
#include "RfAttenuatorTransportSocketLinux.hxx"
#include <microsoft/net/remote/service/RfAttenuatorFactory.hxx>

using namespace std::chrono_literals;

/**
 * @brief Helper class to construct and parse protocol messages for the
 * Aeroflex-Weinschle 83XX model attenuator units.
 *
 * This family of attenuators encode protocol messages as ASCII strings.
 */
struct RfAttenuatorAeroflexWeinschle83XXProtocol
{
    static std::string
    MakeResetRequest();

    static std::string
    MakeGetAttenuatorPropertiesRequest();

    static std::string
    MakeGetAttenuationRequest(uint32_t channel);

    static std::string
    MakeSetAttenuationRequest(uint32_t channel, double attenuation);

    static std::unique_ptr<RfAttenuatorResponseReset>
    ParseResetResponse(const std::vector<std::string>& responses);

    static std::unique_ptr<RfAttenuatorResponseGetProperties>
    ParseGetAttenuatorPropertiesResponse(const std::vector<std::string>& responses);

    static std::unique_ptr<RfAttenuatorResponseGetAttenuation>
    ParseGetAttenuationResponse(const std::vector<std::string>& responses);

    static std::unique_ptr<RfAttenuatorResponseSetAttenuation>
    ParseSetAttenuationResponse(const std::vector<std::string>& responses);
};

struct RfAttenuatorAeroflexWeinschle83XXSocketProtocolAdapter : public RfAttenuatorTransportSocketLinux::ProtocolAdapter
{
    std::vector<uint8_t>
    AdaptRequest(IRfAttenuatorRequest* request) override;

    std::unique_ptr<IRfAttenuatorResponse>
    AdaptResponse(const IRfAttenuatorRequest* request, std::vector<uint8_t>& responseBuffer) override;
};

struct RfAttenuatorAeroflexWeinschle83XXFactory : public IRfAttenuatorWithTcpConnectionFactory
{
    std::unique_ptr<IRfAttenuatorController>
    Create(std::unique_ptr<RfAttenuatorTransportSocketLinux> transportTcp) override;

    TcpTransportConfiguration
    GetTransportConfiguration() override;

    // These values have been determined from experimentation.
    static constexpr uint32_t ReceiveSize{ 1024 };
    static constexpr auto ReceiveDelay{ 1s };
    static constexpr auto SettlingTime{ 1s };

    static constexpr TcpTransportConfiguration TransportConfiguration{
        .ReceiveSize = ReceiveSize,
        .ReceiveDelay = ReceiveDelay,
        .SettlingTime = SettlingTime,
    };

    static constexpr std::array<std::string_view, 2> SupportedNamePrefixes{
        "AeroflexWeinschle83",
        "AFW83",
    };
};
