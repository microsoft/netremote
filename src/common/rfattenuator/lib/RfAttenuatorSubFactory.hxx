#pragma once

#include <chrono>
#include <cstdint>
#include <memory>

#include <microsoft/net/remote/service/RfAttenuator.hxx>

/**
 * @brief Private interface for attenuator implementations that support
 * controlling their properties directly. This is mostly limited to software
 * implementations.
 */
struct IRfAttenuatorBasicFactory
{
    virtual ~IRfAttenuatorBasicFactory() = default;
    /**
     * @brief Create a basic attenuator that does not require any arguments or
     * configuration.
     *
     * @return std::unique_ptr<IRfAttenuatorController>
     */
    virtual std::unique_ptr<IRfAttenuatorController>
    Create(RfAttenuatorProperties properties) = 0;
};
