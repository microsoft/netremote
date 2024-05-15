
#include <array>
#include <cstddef>
#include <filesystem>
#include <format>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string_view>
#include <utility>

#include <wpa_ctrl.h>

#include <Wpa/WpaCommand.hxx>
#include <Wpa/WpaControlSocket.hxx>
#include <Wpa/WpaController.hxx>
#include <Wpa/WpaCore.hxx>
#include <Wpa/WpaResponse.hxx>
#include <magic_enum.hpp>
#include <plog/Log.h>

using namespace Wpa;

WpaController::WpaController(std::string_view interfaceName, WpaType type) :
    WpaController(interfaceName, type, std::filesystem::path(WpaControlSocket::DefaultPath(type)))
{
}

WpaController::WpaController(std::string_view interfaceName, WpaType type, std::string_view controlSocketPath) :
    WpaController(interfaceName, type, std::filesystem::path(controlSocketPath))
{
}

WpaController::WpaController(std::string_view interfaceName, WpaType type, std::filesystem::path controlSocketPath) :
    m_type(type),
    m_interfaceName(interfaceName),
    m_controlSocketPath(std::move(controlSocketPath))
{
}

WpaController::~WpaController()
{
    if (m_controlSocketCommand != nullptr) {
        wpa_ctrl_close(m_controlSocketCommand);
        m_controlSocketCommand = nullptr;
    }
}

WpaType
WpaController::Type() const noexcept
{
    return m_type;
}

std::filesystem::path
WpaController::ControlSocketPath() const noexcept
{
    return m_controlSocketPath;
}

struct wpa_ctrl*
WpaController::GetCommandControlSocket()
{
    // Check if a socket connection is already established.
    {
        const std::shared_lock lockShared{ m_controlSocketCommandGate };
        if (m_controlSocketCommand != nullptr) {
            return m_controlSocketCommand;
        }
    }

    // Check if socket was updated between releasing the shared lock and acquiring the exclusive lock.
    const std::scoped_lock lockExclusive{ m_controlSocketCommandGate };
    if (m_controlSocketCommand != nullptr) {
        return m_controlSocketCommand;
    }

    // Establish a new socket connection. Continue holding the exclusive lock
    // until the connection is established to prevent multiple connections.
    const auto controlSocketPath = m_controlSocketPath / m_interfaceName;
    struct wpa_ctrl* controlSocket = wpa_ctrl_open(controlSocketPath.c_str());
    if (controlSocket == nullptr) {
        LOGE << std::format("Failed to open control socket for {} interface at {}.", m_interfaceName, controlSocketPath.c_str());
        return nullptr;
    }

    LOGD << std::format("Opened {} control socket for {} interface at {}.", magic_enum::enum_name(m_type), m_interfaceName, controlSocketPath.c_str());

    // Update the member and return it.
    m_controlSocketCommand = controlSocket;
    return controlSocket;
}

std::shared_ptr<WpaResponse>
WpaController::SendCommand(const WpaCommand& command)
{
    // Obtain a control socket connection to send the command over.
    struct wpa_ctrl* controlSocket = GetCommandControlSocket();
    if (controlSocket == nullptr) {
        LOGE << std::format("Failed to get control socket for {}.", m_interfaceName);
        return nullptr;
    }

    // Send the command and receive the response.
    std::array<char, WpaControlSocket::MessageSizeMax> responseBuffer{};
    std::size_t responseSize = std::size(responseBuffer);

    auto commandPayload = command.GetPayload();
    LOGD << std::format("Sending wpa command to {} interface: '{}'", m_interfaceName, commandPayload);
    int ret = wpa_ctrl_request(controlSocket, std::data(commandPayload), std::size(commandPayload), std::data(responseBuffer), &responseSize, nullptr);
    switch (ret) {
    case 0: {
        const std::string_view responsePayload{ std::data(responseBuffer), responseSize };
        return command.ParseResponse(responsePayload);
    }
    case -1:
        LOGE << std::format("Failed to send or receive command to/from {} interface.", m_interfaceName);
        return nullptr;
    case -2:
        LOGE << std::format("Sending command to {} interface timed out.", m_interfaceName);
        return nullptr;
    default:
        LOGE << std::format("Unknown error sending command to {} interface (ret={}).", m_interfaceName, ret);
        return nullptr;
    }
}
