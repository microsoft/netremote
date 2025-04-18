#pragma once

#include <arpa/inet.h>
#include <chrono>
#include <cstdint>
#include <span>
#include <string_view>
#include <unistd.h>

#include "RfAttenuatorExceptionImpl.hxx"

using namespace std::chrono_literals;

namespace SocketHelpers
{

struct SocketHelperException : public RfAttenuatorExceptionImpl
{
    SocketHelperException() = default;
    SocketHelperException(std::string what, int errNo);

    int ErrorNo() const noexcept;

private:
    int m_errNo{ 0 };
};

/**
 * @brief Default number of connection attempts to make.
 */
inline constexpr uint32_t NumConnectAttemptsMaxDefault{ 3 };

/**
 * @brief Default amount of time to delay in between connection attempts.
 */
inline constexpr std::chrono::milliseconds ConnectDelayTimeDefault{ 1s };

/**
 * @brief Get the binary representation of the specified IP address.
 *
 * @param ipAddress The string representation of the ip address.
 * @return IN_ADDR The binary representation of the ip address.
 */
struct in_addr
GetBinaryIpAddress(std::string_view ipAddress);

/**
 * @brief Attempt to make a connection to an endpoint on a socket.
 *
 * @param socket The socket to attempt the connection on.
 * @param address The endpoint to attempt to connect to.
 * @param numConnectAttemptsMax The number of connection attempts to make.
 * @param connectAttemptDelayTime The amount of time to delay in between connection attempts.
 */
void
Connect(const int& socket, const struct sockaddr_in& address, uint32_t numConnectAttemptsMax = NumConnectAttemptsMaxDefault, std::chrono::milliseconds connectAttemptDelayTime = ConnectDelayTimeDefault);

/**
 * @brief Transmit data from the specified buffer on the socket.
 *
 * @param socket The socket to transmit the data on.
 * @param buffer The data to transmit.
 */
void
Transmit(const int& socket, std::span<uint8_t> buffer);

/**
 * @brief Receive data on from the socket. This will read as many bytes as are available on the socket, up to the specified buffer length.
 *
 * @param socket The socket to receive data from.
 * @param buffer The buffer to receive data into.
 * @param receiveDelayTime Amount of time to delay between successful reads into the buffer.
 * @return std::size_t The number of bytes read into the buffer.
 */
std::size_t
Receive(const int& socket, std::span<uint8_t> buffer, std::chrono::milliseconds receiveDelayTime);

} // namespace SocketHelpers
