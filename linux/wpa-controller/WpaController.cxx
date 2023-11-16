
#include <Wpa/WpaController.hxx>

using namespace Wpa;

WpaController::WpaController(std::string_view interfaceName, WpaType type) :
    m_type(type),
    m_interfaceName(interfaceName)
{
}

WpaType WpaController::Type() const noexcept
{
    return m_type;
}
