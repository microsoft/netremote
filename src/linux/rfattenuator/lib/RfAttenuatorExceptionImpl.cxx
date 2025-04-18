
#include "RfAttenuatorExceptionImpl.hxx"

RfAttenuatorExceptionImpl::RfAttenuatorExceptionImpl(std::string what) :
    m_what(std::move(what))
{
}

const char*
RfAttenuatorExceptionImpl::what() const noexcept
{
    return m_what.c_str();
}
