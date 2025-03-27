#pragma once

#include <chrono>
#include <cstdint>
#include <memory>

#include <microsoft/net/remote/service/RfAttenuator.hxx>
#include "RfAttenuatorTransportSocketLinux.hxx"

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

/**
 * @brief Aggregated TCP transport configuration.
 */
struct TcpTransportConfiguration
{
    uint32_t ReceiveSize;
    std::chrono::milliseconds ReceiveDelay;
    std::chrono::milliseconds SettlingTime;
};

/**
 * @brief Private interface for attenuator implementations that support
 * TCP-based connections.
 */
struct IRfAttenuatorWithTcpConnectionFactory
{
    /**
     * @brief Create an attenuator which supports TCP-based transports.
     *
     * @param transportTcp The TCP-based transport connection to the attenuator.
     * @return std::unique_ptr<IRfAttenuatorController>
     */
    virtual std::unique_ptr<IRfAttenuatorController>
    Create(std::unique_ptr<RfAttenuatorTransportSocketLinux> transportTcp) = 0;

    virtual ~IRfAttenuatorWithTcpConnectionFactory() = default;

    /**
     * @brief Get the configuration that should be used to create/configure the attenuator's TCP transport.
     *
     * @return TcpTransportConfiguration
     */
    virtual TcpTransportConfiguration
    GetTransportConfiguration() = 0;
};
