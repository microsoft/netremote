#include "RfAttenuatorTransport.hxx"

RfAttenuatorTransportException::RfAttenuatorTransportException(std::string what) :
    RfAttenuatorExceptionImpl(std::move(what))
{
}
