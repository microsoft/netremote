#pragma once

#include <microsoft/net/remote/service/RfAttenuator.hxx>

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

    /**
     * @brief Create an attenuator which uses TCP as the communication transport.
     *
     * @param attenuatorName The name of the attenuator.
     * @param args The arguments needed to establish a TCP connection with the attenuator.
     * @return std::unique_ptr<IRfAttenuatorController>
     */
    static std::unique_ptr<IRfAttenuatorController>
    TryCreateWithTcpConnection(std::string attenuatorName, RfAttenuatorConnectionArgumentsTcp args);
};
