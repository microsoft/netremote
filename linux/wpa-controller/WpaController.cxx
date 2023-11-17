
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
