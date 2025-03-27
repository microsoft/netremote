#include <format>
#include <utility>

#include "LogHelpers.hxx"
#include "RfAttenuatorTransportSocketLinux.hxx"
//#include <pplawait.h>

// namespace winrt
// {
// using winrt::Windows::Foundation::IAsyncAction;
// using winrt::Windows::Foundation::IAsyncOperation;
// using winrt::Windows::Foundation::Collections::IVector;
// } // namespace winrt

/* static */
std::unique_ptr<RfAttenuatorTransportSocketLinux>
RfAttenuatorTransportSocketLinux::CreateWithTcpConnection(
    std::string ipAddress, uint16_t port, uint32_t receiveSize, std::chrono::milliseconds receiveDelay, std::chrono::milliseconds settlingTime)
{
    return std::make_unique<RfAttenuatorTransportSocketLinux>(std::move(ipAddress), port, receiveSize, receiveDelay, settlingTime);
}

RfAttenuatorTransportSocketLinux::RfAttenuatorTransportSocketLinux(std::string ipAddress, uint16_t port, uint32_t receiveSize, std::chrono::milliseconds receiveDelay, std::chrono::milliseconds settlingTime) :
    m_ipAddress(std::move(ipAddress)), m_port(port), m_receiveSize(receiveSize), m_receiveDelay(receiveDelay), m_settlingTime(settlingTime), m_ipv4Address(SocketHelpers::GetBinaryIpAddress(m_ipAddress))
{
    Connect();
}

RfAttenuatorTransportSocketLinux::~RfAttenuatorTransportSocketLinux()
{
    auto traceMe{ LogHelpers::TraceFunction() };
    close(m_socket);
}

/* static */
int
RfAttenuatorTransportSocketLinux::CreateSocket()
{
    auto traceMe{ LogHelpers::TraceFunction() };

    // Create a TCP stream socket with overlapped i/o support.
    auto socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (socketDescriptor<0) {
        throw RfAttenuatorTransportException(std::format("Failed to create transport tcp socket with code 0x{:08x}", errno).c_str());
    }

    return socketDescriptor;
}

void
RfAttenuatorTransportSocketLinux::Connect(OnConnectedAction onConnectedAction)
{
    static constexpr auto ConnectAttemptsMax = 3;
    static constexpr auto ConnectAttemptDelayTime = 1s;

    auto traceMe{ LogHelpers::TraceFunction() };

    if (m_isConnected && (onConnectedAction != OnConnectedAction::Reconnect)) {
        return;
    }

    // Create the socket, replacing any existing one. This is needed to ensure
    // existing connections are severed upon reconnect in cases where only a
    // single connection is supported.
    m_socket = CreateSocket();

    // Prepare a socket address descriptor for the remote endpoint.
    sockaddr_in socketAddressv4{};
    socketAddressv4.sin_family = AF_INET;
    socketAddressv4.sin_port = htons(m_port);
    socketAddressv4.sin_addr = m_ipv4Address;

    // Connect the socket to the remote endpoint.
    SocketHelpers::Connect(m_socket, socketAddressv4, ConnectAttemptsMax, ConnectAttemptDelayTime);

    // Drain any data that may have been sent in response to the connection.
    std::this_thread::sleep_for(m_receiveDelay);
    std::vector<uint8_t> connectBuffer(m_receiveSize);
    [[maybe_unused]] auto bytesReceived = SocketHelpers::Receive(m_socket, std::span(connectBuffer), m_receiveDelay);

    m_isConnected = true;
}

std::vector<uint8_t>
RfAttenuatorTransportSocketLinux::SendRequest(std::vector<uint8_t> request)
{
    auto traceMe{ LogHelpers::TraceFunction() };

    // Send request.
    TransmitRequest(std::span<uint8_t>(request));

    // Wait until the request settles.
    std::this_thread::sleep_for(m_settlingTime);

    // Receive response.
    std::vector<uint8_t> response(m_receiveSize);
    std::span<uint8_t> responseBuffer{ response };
    const auto bytesReceived = ReceiveResponse(responseBuffer);
    if (bytesReceived == 0) {
        throw RfAttenuatorTransportException("socket read failed while receiving response");
    }

    // Resize the buffer based on the actual amount of data read.
    response.resize(bytesReceived);

    // Return the response buffer.
    return response;
}

// concurrency::task<std::vector<uint8_t>>
// RfAttenuatorTransportSocketLinux::SendRequestAsync(std::vector<uint8_t> request)
// {
//     auto traceMe{ LogHelpers::TraceFunction() };

//     return concurrency::create_task([=, request = std::move(request)] {
//         return SendRequest(std::move(request));
//     });
// }

void
RfAttenuatorTransportSocketLinux::TransmitRequest(std::span<uint8_t> requestBuffer)
{
    static constexpr uint32_t NumAttemptsMax = 4;
    static constexpr uint32_t NumAttemptsUntilReconnect = 3;
    static constexpr auto TransmitAttemptDelayTime = 1s;

    auto traceMe{ LogHelpers::TraceFunction() };

    // Ensure the socket is connected.
    Connect(OnConnectedAction::None);

    for (uint32_t numAttempts{ 0 };;) {
        try {
            numAttempts++;
            SocketHelpers::Transmit(m_socket, requestBuffer);
            return;
        } catch (const SocketHelpers::SocketHelperException& e) {
            // If maximum attempts have been reached, give up.
            if (numAttempts >= NumAttemptsMax) {
                throw;
            }

            const auto err = e.ErrorNo();

            // If the socket is not connected, then reconnect and retry.
            if (err == ENOTCONN && numAttempts <= NumAttemptsUntilReconnect) {
                std::cout << "Socket not connected, reconnecting";
                Connect(OnConnectedAction::Reconnect);
                continue;
            }

            // If the socket is not ready for transmit, then delay a little and retry.
            if (err == EWOULDBLOCK || err == ETIMEDOUT) {
                std::this_thread::sleep_for(TransmitAttemptDelayTime);
                continue;
            }

            // Otherwise, give up.
            throw;
        }
    }
}

std::size_t
RfAttenuatorTransportSocketLinux::ReceiveResponse(std::span<uint8_t> responseBuffer)
{
    static constexpr uint32_t NumAttemptsMax = 3;
    static constexpr auto ReceiveAttemptDelayTime = 1s;

    auto traceMe{ LogHelpers::TraceFunction() };

    // Ensure the socket is connected.
    Connect(OnConnectedAction::None);

    for (uint32_t numAttempts{ 0 };;) {
        try {
            numAttempts++;
            auto bytesReceived = SocketHelpers::Receive(m_socket, responseBuffer, m_receiveDelay);
            return bytesReceived;
        } catch (const SocketHelpers::SocketHelperException& e) {
            // If maximum attempts have been reached, give up.
            if (numAttempts >= NumAttemptsMax) {
                throw;
            }

            const auto err = e.ErrorNo();

            // If the socket is not connected, then reconnect and retry. This
            // may not help in cases where the disconnection was caused by the
            // endpoint being reset, and thus, probably lost the state
            // associated with the request. However, later protocol processing
            // of the response will detect this, and so the reconnection is done
            // here as best-effort.
            if (err == ENOTCONN || err == ECONNABORTED || err == ECONNRESET) {
                Connect(OnConnectedAction::Reconnect);
                continue;
            }

            // If data is not available on the socket, then delay a little and retry.
            if (err == EWOULDBLOCK || err == ETIMEDOUT) {
                std::this_thread::sleep_for(ReceiveAttemptDelayTime);
                continue;
            }

            // Otherwise, give up.
            throw;
        }
    }
}
