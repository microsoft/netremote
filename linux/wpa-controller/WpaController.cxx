
#include <cstddef>

#include <wpa_ctrl.h>
#include <Wpa/WpaController.hxx>

using namespace Wpa;

WpaController::WpaController(std::string_view interfaceName, WpaType type) :
    WpaController(std::move(interfaceName), type, WpaControlSocket::DefaultPath(type))
{
}

WpaController::WpaController(std::string_view interfaceName, WpaType type, std::string_view controlSocketPath) :
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
