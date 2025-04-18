
#include "RfAttenuatorProtocol.hxx"

RfAttenuatorRequestException::RfAttenuatorRequestException(std::string what) :
    RfAttenuatorExceptionImpl(std::move(what))
{
}

RfAttenuatorResponseException::RfAttenuatorResponseException(std::string what) :
    RfAttenuatorExceptionImpl(std::move(what))
{
}

IRfAttenuatorRequest::IRfAttenuatorRequest(RfAttenuatorRequestType type) :
    Type(type)
{
}

RfAttenuatorRequestReset::RfAttenuatorRequestReset() :
    IRfAttenuatorRequest(RfAttenuatorRequestType::Reset)
{
}

/* static */
std::unique_ptr<RfAttenuatorRequestReset>
RfAttenuatorRequestReset::Make()
{
    return std::make_unique<RfAttenuatorRequestReset>();
}

RfAttenuatorRequestGetProperties::RfAttenuatorRequestGetProperties() :
    IRfAttenuatorRequest(RfAttenuatorRequestType::GetAttenuatorProperties)
{
}

/* static */
std::unique_ptr<RfAttenuatorRequestGetProperties>
RfAttenuatorRequestGetProperties::Make()
{
    return std::make_unique<RfAttenuatorRequestGetProperties>();
}

/* static */
std::unique_ptr<RfAttenuatorRequestGetAttenuation>
RfAttenuatorRequestGetAttenuation::Make(uint32_t channel)
{
    return std::make_unique<RfAttenuatorRequestGetAttenuation>(channel);
}

RfAttenuatorRequestGetAttenuation::RfAttenuatorRequestGetAttenuation(uint32_t channel) :
    IRfAttenuatorRequest(RfAttenuatorRequestType::GetAttenuationValue), Channel(channel)
{
}

RfAttenuatorResponseGetAttenuation::RfAttenuatorResponseGetAttenuation(RfAttenuationChannelStatus attenuationStatus) :
    AttenuationStatus(std::move(attenuationStatus))
{
}

/* static */
std::unique_ptr<RfAttenuatorRequestSetAttenuation>
RfAttenuatorRequestSetAttenuation::Make(uint32_t channel, double attenuation)
{
    return std::make_unique<RfAttenuatorRequestSetAttenuation>(channel, attenuation);
}

RfAttenuatorRequestSetAttenuation::RfAttenuatorRequestSetAttenuation(uint32_t channel, double attenuation) :
    IRfAttenuatorRequest(RfAttenuatorRequestType::SetAttenuationValue), Channel(channel), Attenuation(attenuation)
{
}
