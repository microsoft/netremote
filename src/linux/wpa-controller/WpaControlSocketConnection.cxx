
#include <filesystem>
#include <memory>

#include <notstd/Memory.hxx>
#include <wpa_ctrl.h>

#include <Wpa/WpaControlSocket.hxx>
#include <Wpa/WpaControlSocketConnection.hxx>

using namespace Wpa;

WpaControlSocketConnection::~WpaControlSocketConnection()
{
    Disconnect();
}

WpaControlSocketConnection::WpaControlSocketConnection(std::string_view interfaceName, std::filesystem::path controlSocketPathDir) :
    m_controlSocketPath(std::move(controlSocketPathDir) / interfaceName)
{
}

std::unique_ptr<WpaControlSocketConnection>
WpaControlSocketConnection::TryCreate(std::string_view interfaceName, std::filesystem::path controlSocketPathDir)
{
    auto controlSocketConnection = std::make_unique<notstd::enable_make_protected<WpaControlSocketConnection>>(interfaceName, std::move(controlSocketPathDir));
    if (!controlSocketConnection->Connect()) [[unlikely]] {
        return nullptr;
    }

    return controlSocketConnection;
}

WpaControlSocketConnection::operator struct wpa_ctrl *() const noexcept
{
    return m_controlSocket;
}

bool
WpaControlSocketConnection::Connect(bool forceReconnect) noexcept
{
    if (m_controlSocket != nullptr) {
        if (!forceReconnect) {
            return true;
        }

        Disconnect();
    }

    auto controlSocket = wpa_ctrl_open(m_controlSocketPath.c_str());
    if (controlSocket != nullptr) {
        m_controlSocket = controlSocket;
    }

    return (m_controlSocket != nullptr);
}

void
WpaControlSocketConnection::Disconnect() noexcept
{
    if (m_controlSocket != nullptr) [[likely]] {
        wpa_ctrl_close(m_controlSocket);
        m_controlSocket = nullptr;
    }
}
