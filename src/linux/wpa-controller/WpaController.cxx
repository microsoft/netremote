
#include <array>
#include <cstddef>
#include <format>
#include <iostream>
#include <mutex>

#include <magic_enum.hpp>
#include <wpa_ctrl.h>
#include <Wpa/WpaController.hxx>

using namespace Wpa;

WpaController::WpaController(std::string_view interfaceName, WpaType type) :
    WpaController(std::move(interfaceName), type, std::filesystem::path(WpaControlSocket::DefaultPath(type)))
{
}

WpaController::WpaController(std::string_view interfaceName, WpaType type, std::string_view controlSocketPath) :
    WpaController(std::move(interfaceName), type, std::filesystem::path(controlSocketPath))
{
}

WpaController::WpaController(std::string_view interfaceName, WpaType type, std::filesystem::path controlSocketPath) :
    m_type(type),
    m_interfaceName(interfaceName),
    m_controlSocketPath(controlSocketPath)
{
}

WpaType WpaController::Type() const noexcept
{
    return m_type;
}

std::filesystem::path WpaController::ControlSocketPath() const noexcept
{
    return m_controlSocketPath;
}

struct wpa_ctrl*
WpaController::GetCommandControlSocket()
{
    // Check if a socket connection is already established.
    {
        std::shared_lock lockShared{m_controlSocketCommandGate};
        if (m_controlSocketCommand != nullptr)
        {
            return m_controlSocketCommand;
        }
    }

    // Check if socket was updated between releasing the shared lock and acquiring the exclusive lock.
    std::scoped_lock lockExclusive{ m_controlSocketCommandGate };
    if (m_controlSocketCommand != nullptr)
    {
        return m_controlSocketCommand;
    }

    // Establish a new socket connection. Continue holding the exclusive lock
    // until the connection is established to prevent multiple connections.
    const auto controlSocketPath = m_controlSocketPath / m_interfaceName;
    struct wpa_ctrl* controlSocket = wpa_ctrl_open(controlSocketPath.c_str());
    if (controlSocket == nullptr)
    {
        std::cerr << std::format("Failed to open control socket for {} interface at {}.", m_interfaceName, controlSocketPath.c_str()) << std::endl;
        return nullptr;
    }

    // Update the member and return it.
    m_controlSocketCommand = controlSocket;
    return controlSocket;
}

std::shared_ptr<WpaResponse>
WpaController::SendCommand(const WpaCommand& command)
{
    // Obtain a control socket connection to send the command over.
    struct wpa_ctrl* controlSocket = GetCommandControlSocket();
    if (controlSocket == nullptr)
    {
        std::cerr << std::format("Failed to get control socket for {}.", m_interfaceName) << std::endl;
        return nullptr;
    }

    // Send the command and receive the response.
    std::array<char, WpaControlSocket::MessageSizeMax> responseBuffer;
    std::size_t responseSize = std::size(responseBuffer);

    int ret = wpa_ctrl_request(controlSocket, std::data(command.Payload), std::size(command.Payload), std::data(responseBuffer), &responseSize, nullptr);
    switch (ret)
    {
    case 0:
    {
        std::string_view responsePayload{std::data(responseBuffer), responseSize};
        return command.ParseResponse(responsePayload);
    }
    case -1:
        std::cerr << std::format("Failed to send or receive command to {} interface.", m_interfaceName) << std::endl;
        return nullptr;
    case -2:
        std::cerr << std::format("Sending command to {} interface timed out.", m_interfaceName) << std::endl;
        return nullptr;
    default:
        std::cerr << std::format("Unknown error sending command to {} interface (ret={}).", m_interfaceName, ret) << std::endl;
        return nullptr;
    }
}
