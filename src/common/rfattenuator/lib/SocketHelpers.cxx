

#include <format>
#include <thread>

#include "LogHelpers.hxx"
#include "SocketHelpers.hxx"

// #include <wil/result.h>

// #include <WS2tcpip.h>

namespace detail
{
// Timeout to wait for a socket to become ready for Transmit (send()) operation.
timeval TransmitReadyTimeout{
    .tv_sec = 1, // 1 second
    .tv_usec = 0
};

// Timeout to wait for a socket to become ready for Receive (recv()) operation.
timeval ReceiveReadyTimeout = TransmitReadyTimeout;
} // namespace detail

namespace SocketHelpers
{

SocketHelperException::SocketHelperException(std::string what, int errNo) :
    RfAttenuatorExceptionImpl(std::move(what)),
    m_errNo(errNo)
{
}

int
SocketHelperException::ErrorNo() const noexcept
{
    return m_errNo;
}

struct in_addr
GetBinaryIpAddress(std::string_view ipAddress)
{
    struct in_addr ipv4Address{};
    auto ret = inet_pton(AF_INET, std::data(ipAddress), &ipv4Address);
    if (ret != 1) {
        throw SocketHelperException(std::format("Failed to convert ip address {} to binary format with errno {:#010x}", ipAddress, ret).c_str(), errno);
    }

    return ipv4Address;
}

void
Connect(const int& socket, const struct sockaddr_in& address, uint32_t numConnectAttemptsMax, std::chrono::milliseconds connectAttemptDelayTime)
{
    auto traceMe{ LogHelpers::TraceFunction() };

    // Connect the socket to the endpoint.
    for (uint32_t numConnectAttempts{ 0 }; numConnectAttempts < numConnectAttemptsMax; numConnectAttempts++) {
        auto ret = connect(socket, reinterpret_cast<const struct sockaddr*>(&address), sizeof address);
        if (ret == 0) {
            std::cout << "Connected to socket";
            return;
        }

        // If there was a timeout or the remote endpoint refused to connect
        // (possibly not ready yet), then delay a little and retry if there are
        // attempts remaining.
        if (errno == ECONNREFUSED || ret == ETIMEDOUT) {
            std::this_thread::sleep_for(connectAttemptDelayTime);
            continue;
        }
        // Otherwise bail out on unrecoverable errors.
        else {
            throw SocketHelperException(std::format("Failed to connect tcp socket to endpoint after {} attempts with errno {:#010x}", numConnectAttempts, errno).c_str(), errno);
        }
    }

    throw SocketHelperException(std::format("Failed to connect tcp socket to endpoint after {} attempts; giving up", numConnectAttemptsMax).c_str(), ETIMEDOUT);
}

void
Transmit(const int& socket, std::span<uint8_t> buffer)
{
    auto traceMe{ LogHelpers::TraceFunction() };

    for (;;) {
        // Check if the buffer has un-transmitted bytes remaining.
        const auto bytesToTransmit = buffer.size_bytes();
        if (bytesToTransmit == 0) {
            break;
        }

        // Prepare a socket descriptor set to wait on write availability.
        fd_set writeSet{};
        FD_SET(socket, &writeSet);

        // Wait for socket write buffer to become ready for data.
        const auto numSocketsReady = select(0, nullptr, &writeSet, nullptr, &detail::TransmitReadyTimeout);
        if (numSocketsReady < 0) {
            throw SocketHelperException(std::format("Error while waiting for socket write buffer to become ready for data with errno=0x{:08x}", errno).c_str(), errno);
        } else if (numSocketsReady == 0) {
            throw SocketHelperException("Timeout while waiting for socket write buffer to become ready for data", ETIMEDOUT);
        }

        // Attempt to transmit the remaining buffer.
        const auto bytesTransmitted = send(socket, reinterpret_cast<const char*>(std::data(buffer)), bytesToTransmit, 0);
        if (bytesTransmitted <= 0) {
            throw SocketHelperException(std::format("send() of {} bytes failed with errno=0x{:08x}", bytesToTransmit, errno).c_str(), errno);
        }

        // Advance the buffer to the un-transmitted portions.
        buffer = buffer.subspan(static_cast<std::size_t>(bytesTransmitted));
    }
}

std::size_t
Receive(const int& socket, std::span<uint8_t> buffer, std::chrono::milliseconds receiveDelayTime)
{
    auto traceMe{ LogHelpers::TraceFunction() };

    std::size_t bytesReceivedTotal = 0;

    for (;;) {
        // Check if the buffer has space to receive more data.
        const auto bytesToReceive = buffer.size_bytes();
        if (bytesToReceive == 0) {
            break;
        }

        // Prepare a socket descriptor set to wait on read data.
        fd_set readSet{};
        FD_SET(socket, &readSet);

        // Wait for data to become available for reading on the socket.
        const auto numSocketsReady = select(0, &readSet, nullptr, nullptr, &detail::ReceiveReadyTimeout);
        if (numSocketsReady < 0) {
            throw SocketHelperException(std::format("Error while waiting for data to become available with errno=0x{:08x}", errno).c_str(), errno);
        } else if (numSocketsReady == 0) {
            // The socket has no read data available following the timeout
            // period, so assume we've read all the data and exit the read
            // loop.
            break;
        }

        // Read all available data up to the buffer size.
        const auto bytesReceived = recv(socket, reinterpret_cast<char*>(std::data(buffer)), bytesToReceive, 0);
        if (bytesReceived < 0) {
            throw SocketHelperException(std::format("recv() of {} bytes failed with errno=0x{:08x}", bytesToReceive, errno).c_str(), errno);
        } else if (bytesReceived == 0) {
            // Connection was gracefully closed so exit the read loop to avoid
            // attempting to read any more data.
            break;
        }

        // Advance the buffer beyond the data just read.
        bytesReceivedTotal += static_cast<std::size_t>(bytesReceived);
        buffer = buffer.subspan(static_cast<std::size_t>(bytesReceived));

        // Sleep for a bit because the hardware bugs out otherwise :'(
        std::this_thread::sleep_for(receiveDelayTime);
    }

    return bytesReceivedTotal;
}

} // namespace SocketHelpers
