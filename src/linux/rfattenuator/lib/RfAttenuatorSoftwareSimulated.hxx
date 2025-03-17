#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "include/RfAttenuatorFactory.hxx"
#include "RfAttenuatorSubFactory.hxx"

/**
 * @brief Software-simulated RF attenuator.
 *
 * This is a pure software implementation that may be used for testing purposes.
 */
struct RfAttenuatorSoftwareSimulated : public IRfAttenuatorController
{
    /**
     * @brief Construct a new RfAttenuatorSoftwareSimulated object with the specified
     * properties.
     *
     * @param properties The properties the attenuator should support.
     */
    RfAttenuatorSoftwareSimulated(RfAttenuatorProperties properties);

    /**
     * @brief Reset all attenuator state.
     */
    void
    Reset() override;

    /**
     * @brief Get the fixed properties of the attenuator.
     *
     * @return RfAttenuatorProperties
     */
    RfAttenuatorProperties
    GetProperties() override;

    /**
     * @brief Get the attenuation for a channel.
     *
     * Note that channel indexing begins at 0, not 1.
     *
     * @param channel The channel to obtain attenuation for.
     * @return double The current attenuation for the channel.
     */
    double
    GetAttenuationForChannel(uint32_t channel) override;

    /**
     * @brief Set the Attenuation For Channel object
     *
     * @param channel The channel to set attenuation for.
     * @param attenuation The attenuation to set on the channel.
     * @return true If the attenuation was updated successfully.
     * @return false If the attenuation was not updated successfully.
     */
    bool
    SetAttenuationForChannel(uint32_t channel, double attenuation) override;

private:
    /**
     * @brief Resets all channel settings to defaults.
     *
     * Note: An exclusive lock of m_channelGate must be held when calling this function.
     */
    void
    ResetChannelSettings();

private:
    struct ChannelSettings
    {
        double Attenuation;
    };

    RfAttenuatorProperties m_properties;
    std::unordered_map<uint32_t, ChannelSettings> m_channels;
    std::mutex m_channelsGate;

    static constexpr auto DefaultIdentification = "Software Simulated RF Attenuator";
};

struct RfAttenuatorSoftwareSimulatedFactory : public IRfAttenuatorBasicFactory
{
    std::unique_ptr<IRfAttenuatorController>
    Create(RfAttenuatorProperties properties) override;
};
