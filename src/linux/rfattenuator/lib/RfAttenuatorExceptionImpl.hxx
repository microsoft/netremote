#pragma once

#include <string>

#include "include/RfAttenuator.hxx"

/**
 * @brief Primary base implementation of RfAttenuatorException. Sub-classing of
 * exceptions should be done with this class as the parent.
 */
struct RfAttenuatorExceptionImpl : public RfAttenuatorException
{
    virtual ~RfAttenuatorExceptionImpl() = default;
    RfAttenuatorExceptionImpl() = default;
    RfAttenuatorExceptionImpl(std::string what);

    const char*
    what() const noexcept override;

private:
    std::string m_what{};
};
