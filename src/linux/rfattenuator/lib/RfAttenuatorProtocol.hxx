#pragma once

#include <cstdint>
#include <exception>
#include <memory>
#include <optional>
#include <string>

#include "RfAttenuator.hxx"
#include "RfAttenuatorExceptionImpl.hxx"

/**
 * @brief The status of an attenuator request operation.
 */
enum class RfAttenuatorRequestStatus {
    Succeeded,
    Failed,
};

/**
 * @brief Type of attenuator request.
 */
enum class RfAttenuatorRequestType {
    Reset,
    GetAttenuatorProperties,
    GetAttenuationValue,
    SetAttenuationValue,
};

/**
 * @brief Base class for all exceptions that can result from sending a request
 * to an attenuator.
 */
struct RfAttenuatorRequestException : public RfAttenuatorExceptionImpl
{
    RfAttenuatorRequestException() = default;
    RfAttenuatorRequestException(std::string what);
};

/**
 * @brief Base class for all exceptions that can result from receiving a
 * response from an attenuator.
 */
struct RfAttenuatorResponseException : public RfAttenuatorExceptionImpl
{
    RfAttenuatorResponseException() = default;
    RfAttenuatorResponseException(std::string what);
};

/**
 * @brief Base class (header) for all protocol agnostic requests that can be
 * sent to an attenuator.
 */
struct IRfAttenuatorRequest
{
    virtual ~IRfAttenuatorRequest() = default;
    RfAttenuatorRequestType Type;

protected:
    IRfAttenuatorRequest(RfAttenuatorRequestType type);
};

/**
 * @brief Base class (header) for all protocol agnostic responses that can be
 * sent from an attenuator.
 */
struct IRfAttenuatorResponse
{
    /**
     * @brief Cookie value used to validate correct protocol adaption for
     * responses in the absence of RTTI.
     */
    static constexpr uint32_t CookieValue = 0xDEADBEEF;

    virtual ~IRfAttenuatorResponse() = default;

    const uint32_t Cookie{ CookieValue };
    RfAttenuatorRequestStatus Status{ RfAttenuatorRequestStatus::Failed };
    std::optional<std::string> ErrorDetails;
};

/**
 * @brief Describes the status of a particular attenuator channel.
 */
struct RfAttenuationChannelStatus
{
    uint32_t Channel;
    double Attenuation;
};

/**
 * @brief Request to reset attenuator hardware and clear state.
 */
struct RfAttenuatorRequestReset : public IRfAttenuatorRequest
{
    static std::unique_ptr<RfAttenuatorRequestReset>
    Make();

    RfAttenuatorRequestReset();
    // No input arguments, so no additional fields needed.
};

/**
 * @brief Request to reset attenuator hardware and clear state.
 */
struct RfAttenuatorResponseReset : public IRfAttenuatorResponse
{
};

/**
 * @brief Request to obtain the current, effective properties of the attenuator.
 */
struct RfAttenuatorRequestGetProperties : public IRfAttenuatorRequest
{
    static std::unique_ptr<RfAttenuatorRequestGetProperties>
    Make();

    RfAttenuatorRequestGetProperties();
    // No input arguments, so no additional fields needed.
};

/**
 * @brief Response message for RfAttenuatorRequestGetProperties request.
 */
struct RfAttenuatorResponseGetProperties : public IRfAttenuatorResponse
{
    RfAttenuatorProperties AttenuatorProperties;
};

/**
 * @brief Request to obtain current attenuation status of a particular
 * attenuator channel.
 */
struct RfAttenuatorRequestGetAttenuation : public IRfAttenuatorRequest
{
    static std::unique_ptr<RfAttenuatorRequestGetAttenuation>
    Make(uint32_t channel);

    RfAttenuatorRequestGetAttenuation(uint32_t channel);

    uint32_t Channel;
};

/**
 * @brief Response message for RfAttenuatorRequestGetAttenuation request.
 */
struct RfAttenuatorResponseGetAttenuation : public IRfAttenuatorResponse
{
    RfAttenuatorResponseGetAttenuation(RfAttenuationChannelStatus attenuationStatus);

    RfAttenuationChannelStatus AttenuationStatus;
};

/**
 * @brief Request to set current attenuation of a particular attenuator channel.
 */
struct RfAttenuatorRequestSetAttenuation : public IRfAttenuatorRequest
{
    static std::unique_ptr<RfAttenuatorRequestSetAttenuation>
    Make(uint32_t channel, double attenuation);

    RfAttenuatorRequestSetAttenuation(uint32_t channel, double attenuation);

    uint32_t Channel;
    double Attenuation;
};

/**
 * @brief Response message for RfAttenuatorResponseGetAttenuation request.
 */
struct RfAttenuatorResponseSetAttenuation : public IRfAttenuatorResponse
{
    RfAttenuatorResponseSetAttenuation() = default;
};
