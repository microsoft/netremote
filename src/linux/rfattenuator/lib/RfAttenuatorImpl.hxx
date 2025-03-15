#pragma once

#include <cstdint>
#include <format>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

#include "RfAttenuator.hxx"
#include "RfAttenuatorProtocol.hxx"
#include "RfAttenuatorTransport.hxx"

/**
 * @brief Generic base class that implements the primary IRfAttenuatorController
 * API, tying together the transport and protocol adaption internal components,
 * allowing them to be implemented without dependencies on each other.
 *
 * @tparam TransportType The data type of messages sent over the transport.
 */
template <typename TransportType>
struct RfAttenuator : public IRfAttenuatorController
{
    using Transport = RfAttenuatorTransport<TransportType>;
    using ProtocolAdapter = typename Transport::ProtocolAdapter;

    /**
     * @brief Construct a new RfAttenuator object instance.
     *
     * @param transport The transport used by this attenuator.
     * @param protocolAdapter The desired protocol adaption for this attenuator.
     */
    RfAttenuator(std::unique_ptr<Transport> transport, std::unique_ptr<ProtocolAdapter> protocolAdapter) :
        m_transport(std::move(transport)), m_protocolAdapter(std::move(protocolAdapter))
    {
    }
    ~RfAttenuator() override
    {
        auto traceMe{ LogHelpers::TraceFunction() };
    }

    /**
     * @brief Issue a request and return the corresponding API-typed
     * (IRfAttenuatorController::*) return value.
     *
     * This helper function helps to create requests, adapt them for the
     * selected protocol, exchange the request and response messages over
     * the transport, and extract the API-typed return value.
     *
     * @tparam ReturnValueType The return value type expected for the corresponding API call.
     * @param buildRequest The function which builds the protocol-agnostic request message.
     * @param getResult The function which derives the API-typed return value from the response.
     * @return std::tuple<std::unique<IRfAttenuatorResponse>, ReturnValueType> A
     * tuple containing the protocol-agnostic response, and the return value of
     * the corresponding API call if the request succeeded.
     */
    template <typename ReturnValueType>
    std::tuple<std::unique_ptr<IRfAttenuatorResponse>, ReturnValueType>
    IssueRequest(
        std::function<std::unique_ptr<IRfAttenuatorRequest>()> buildRequest, std::function<ReturnValueType(IRfAttenuatorResponse*)> getResult)
    {
        // Validate required input arguments for creating request and obtaining result.
        if (buildRequest == nullptr || getResult == nullptr) {
            throw std::runtime_error("implementation must provide functions to build request and obtain API result");
        }

        // Create protocol-specific request message for transport.
        auto request = buildRequest();
        auto requestProtocol = m_protocolAdapter->AdaptRequest(request.get());

        // Send the protocol-specific message over the transport.
        auto responseProtocol = m_transport->SendRequest(std::move(requestProtocol));

        // Convert the protocol-specific response to transport-agnostic response message and validate basic sane response.
        auto response = m_protocolAdapter->AdaptResponse(request.get(), responseProtocol);
        if (response->Cookie != decltype(response)::element_type::CookieValue) {
            throw RfAttenuatorRequestException("protocol adapter implementation return the wrong type; cookie validation failed");
        }
        // Validate the request was completed successfully.
        else if (response->Status != RfAttenuatorRequestStatus::Succeeded) {
            return std::make_tuple(std::move(response), ReturnValueType{});
        }

        // Convert protocol message response to API result type.
        auto result = getResult(response.get());
        return std::make_tuple(std::move(response), result);
    }

    void
    Reset() override
    {
        auto [resetResponse, resetSucceeded] = IssueRequest<bool>(
            []() {
                return RfAttenuatorRequestReset::Make();
            },
            [&](IRfAttenuatorResponse* response) {
                const auto responseReset = static_cast<RfAttenuatorResponseReset*>(response);
                return (responseReset->Status == RfAttenuatorRequestStatus::Succeeded);
            });

        if (!resetSucceeded) {
            throw RfAttenuatorResponseException("Reset request failed");
        }
    }

    RfAttenuatorProperties
    GetProperties() override
    {
        auto [getPropertiesResponse, attenuatorProperties] = IssueRequest<RfAttenuatorProperties>(
            []() {
                return RfAttenuatorRequestGetProperties::Make();
            },
            [&](IRfAttenuatorResponse* response) {
                auto responseGetProperties = static_cast<RfAttenuatorResponseGetProperties*>(response);
                return std::move(responseGetProperties->AttenuatorProperties);
            });

        if (getPropertiesResponse->Status != RfAttenuatorRequestStatus::Succeeded) {
            throw RfAttenuatorResponseException("GetProperties request failed");
        }

        return attenuatorProperties;
    }

    double
    GetAttenuationForChannel(uint32_t channel) override
    {
        auto [getAttenuationResponse, attenuation] = IssueRequest<double>(
            [&]() {
                return RfAttenuatorRequestGetAttenuation::Make(channel);
            },
            [&](IRfAttenuatorResponse* response) {
                const auto* responseGetAttenuation = static_cast<RfAttenuatorResponseGetAttenuation*>(response);
                if (responseGetAttenuation->AttenuationStatus.Channel != channel) {
                    throw RfAttenuatorRequestException(std::format(
                        "channel mismatch in GetAttenuation request; expected {} got {}",
                        channel,
                        responseGetAttenuation->AttenuationStatus.Channel));
                }
                return responseGetAttenuation->AttenuationStatus.Attenuation;
            });

        if (getAttenuationResponse->Status != RfAttenuatorRequestStatus::Succeeded) {
            throw RfAttenuatorResponseException("GetAttenuationForChannel request failed");
        }

        return attenuation;
    }

    bool
    SetAttenuationForChannel(uint32_t channel, double attenuation) override
    {
        auto [setAttenuationResponse, attenuationForChannelUpdated] = IssueRequest<bool>(
            [&]() {
                return RfAttenuatorRequestSetAttenuation::Make(channel, attenuation);
            },
            [&](IRfAttenuatorResponse* response) {
                const auto* responseSetAttenuation = static_cast<RfAttenuatorResponseSetAttenuation*>(response);
                return (responseSetAttenuation->Status == RfAttenuatorRequestStatus::Succeeded);
            });

        return attenuationForChannelUpdated;
    }

protected:
    std::unique_ptr<Transport> m_transport;
    std::unique_ptr<ProtocolAdapter> m_protocolAdapter;
};
