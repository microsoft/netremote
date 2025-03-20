#pragma once

#include "RfAttenuator.hxx"

#include <memory>
#include <string>

/**
 * @brief Factory class for creating attenuator controller instances.
 */
struct RfAttenuatorFactory
{
    /**
     * @brief Create a basic attenuator that does not require configuration.
     *
     * @param attenuatorName
     * @param properties The properties the attenuator should support.
     * @return std::unique_ptr<IRfAttenuatorController>
     */
    static std::unique_ptr<IRfAttenuatorController>
    TryCreateBasic(std::string attenuatorName, RfAttenuatorProperties properties);
};
