#pragma once

#include <chrono>
#include <cstdint>
#include <memory>
#include <span>
#include <vector>

// #include <WinSock2.h>
// #include <wil/resource.h>

//#include <ppltasks.h>
// #include <winrt/windows.foundation.collections.h>
// #include <winrt/windows.foundation.h>

#include "RfAttenuatorTransport.hxx"
#include "SocketHelpers.hxx"

using namespace std::chrono_literals;

/**
 * @brief Win32 Socket-based attenuator transport.
 */
struct RfAttenuatorTransportSocketLinux : public RfAttenuatorTransport<std::vector<uint8_t>>
{
    static std::unique_ptr<RfAttenuatorTransportSocketLinux>
    CreateWithTcpConnection(std::string ipAddress, uint16_t port, uint32_t receiveSize = ReceiveSizeDefault, std::chrono::milliseconds receiveDelay = ReceiveDelayDefault, std::chrono::milliseconds settlingTime = SettlingTimeDefault);

    RfAttenuatorTransportSocketLinux(std::string ipAddress, uint16_t port, uint32_t receiveSize, std::chrono::milliseconds receiveDelay, std::chrono::milliseconds settlingTime);
    ~RfAttenuatorTransportSocketLinux() override;
    std::vector<uint8_t>
    SendRequest(std::vector<uint8_t> request) override;

    // concurrency::task<std::vector<uint8_t>>
    // SendRequestAsync(std::vector<uint8_t> request) override;

private:
    static int
    CreateSocket();

    enum class OnConnectedAction {
        None,
        Reconnect,
    };

    void
    Connect(OnConnectedAction onConnectedAction = OnConnectedAction::None);

    void
    TransmitRequest(std::span<uint8_t> requestBuffer);

    std::size_t
    ReceiveResponse(std::span<uint8_t> responseBuffer);

private:
    static constexpr uint32_t ReceiveSizeDefault{ 1024 };
    static constexpr auto ReceiveDelayDefault{ 0ms };
    static constexpr auto SettlingTimeDefault{ 0s };

    const std::string m_ipAddress;
    const uint16_t m_port;
    const uint32_t m_receiveSize{ ReceiveSizeDefault };
    const std::chrono::milliseconds m_receiveDelay{ ReceiveDelayDefault };
    const std::chrono::milliseconds m_settlingTime{ SettlingTimeDefault };

    struct in_addr m_ipv4Address{};
    int m_socket;
    bool m_isConnected{ false };
};
