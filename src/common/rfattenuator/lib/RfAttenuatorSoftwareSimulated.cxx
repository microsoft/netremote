
#include "RfAttenuatorSoftwareSimulated.hxx"

#include <exception>
#include <format>

std::unique_ptr<IRfAttenuatorController> RfAttenuatorSoftwareSimulatedFactory::Create(RfAttenuatorProperties properties)
{
    return std::make_unique<RfAttenuatorSoftwareSimulated>(std::move(properties));
}

RfAttenuatorSoftwareSimulated::RfAttenuatorSoftwareSimulated(RfAttenuatorProperties properties) :
    m_properties(std::move(properties)), m_channels{std::size(m_properties.Channels)}
{
    // Note: no lock needed since object isn't constructed yet, thus no races are possible.
    ResetChannelSettings();

    if (!m_properties.Identification.has_value())
    {
        m_properties.Identification = DefaultIdentification;
    }
}

void RfAttenuatorSoftwareSimulated::Reset()
{
    const std::scoped_lock channelsLock{m_channelsGate};
    ResetChannelSettings();
}

RfAttenuatorProperties RfAttenuatorSoftwareSimulated::GetProperties()
{
    return m_properties;
}

double RfAttenuatorSoftwareSimulated::GetAttenuationForChannel(uint32_t channel)
{
    const std::scoped_lock channelsLock{m_channelsGate};

    auto channelSettingsNode = m_channels.extract(channel);
    if (channelSettingsNode.empty())
    {
        throw std::out_of_range(std::format("invalid channel number '{}' specified", channel));
    }

    auto& channelSettings = channelSettingsNode.mapped();
    auto attenuation = channelSettings.Attenuation;
    m_channels.insert(std::move(channelSettingsNode));

    return attenuation;
}

bool RfAttenuatorSoftwareSimulated::SetAttenuationForChannel(uint32_t channel, double attenuation)
{
    if (attenuation < m_properties.AttenuationRangeDbmMin || attenuation > m_properties.AttenuationRangeDbmMax)
    {
        throw std::invalid_argument(std::format(
            "attenuation value out of range; expected range {}-{}", m_properties.AttenuationRangeDbmMin, m_properties.AttenuationRangeDbmMax));
    }

    const std::scoped_lock channelsLock{m_channelsGate};

    auto channelSettingsNode = m_channels.extract(channel);
    if (channelSettingsNode.empty())
    {
        throw std::out_of_range(std::format("invalid channel number '{}' specified", channel));
    }

    auto& channelSettings = channelSettingsNode.mapped();
    channelSettings.Attenuation = attenuation;
    m_channels.insert(std::move(channelSettingsNode));

    return true;
}

void RfAttenuatorSoftwareSimulated::ResetChannelSettings()
{
    m_channels.clear();

    for (const auto& channel : m_properties.Channels)
    {
        m_channels.emplace(channel, 0.0);
    }
}
